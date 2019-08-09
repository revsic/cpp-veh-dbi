#include <Windows.h>
#include <DbgHelp.h>

#include <asm_support.hpp>
#include <branch_tracer.hpp>

std::once_flag BranchTracer::init_sym;

// Constructor.
BranchTracer::BranchTracer(size_t start, size_t end, std::unique_ptr<BTCallback> callback) :
    start(start), end(end), callback(std::move(callback))
{
    std::call_once(init_sym, []{ SymInitialize(GetCurrentProcess(), NULL, TRUE); });
}

// Text section based tracer.
BranchTracer::BranchTracer(std::unique_ptr<BTCallback> callback) : callback(std::move(callback)) {
    auto[text_start, text_end] = Utils::GetTextSectionAddress();
    start = text_start;
    end = text_end;

    std::call_once(init_sym, []{ SymInitialize(GetCurrentProcess(), NULL, TRUE); });
}

// Handle single step exception.
void BranchTracer::HandleSingleStep(PCONTEXT context, Utils::SoftwareBP& bp) {
    Trace(context, bp);
}

// Handle software breakpoint exception.
void BranchTracer::HandleBreakpoint(PCONTEXT context, Utils::SoftwareBP& bp) {
    bp.Recover(context->RegisterIp);
    Trace(context, bp);
}

// Trace given context.
void BranchTracer::Trace(PCONTEXT context, Utils::SoftwareBP& bp) {
    bool bp_set = false;
    auto jmp_call = [](BYTE* opc) {
        if (opc[0] == 0xFF) {
            BYTE reg = (opc[1] >> 0x3) & 0x7;
            // Binary 010(near call) or 011(far call) 
            //        100(near jmp)  or 101(far jmp)
            return reg >= 2 && reg <= 5;
        }
        return false;
    };

    auto bp_on_retn = [&, this](size_t called, size_t retn) {
        if (!(start <= called && called <= end)) {
            bp.Set(retn);
            bp_set = true;
        }
    };

    BTInfo info;
    BYTE* opc = reinterpret_cast<BYTE*>(context->RegisterIp);

    // instruction call
    if (opc[0] == 0xE8) {
        size_t called = context->RegisterIp + *reinterpret_cast<long*>(opc + 1) + 5;
        BYTE* called_opc = reinterpret_cast<BYTE*>(called);

        // set brancing information
        info = BTInfo{context->RegisterIp, called, context->RegisterIp + 5, true, false};

        // if instruction jump to windows api
        if (jmp_call(called_opc)) {
            auto[api, retn] = ASMSupport::GetBranchingAddress(called_opc, context);
            bp_on_retn(api, context->RegisterIp + 5);

            // set for ff branch
            info.ff_branch = true;
            info.called = api;
        }
    } else if (jmp_call(opc)) {
        auto[called, retn] = ASMSupport::GetBranchingAddress(opc, context);
        bp_on_retn(called, retn);

        // set brancing information
        info = BTInfo{context->RegisterIp, called, retn, false, true};
    }

    // callback
    if (callback != nullptr) {
        callback->run(info, context);
    }

    if (!bp_set) {
        Utils::SetSingleStep(context);
    }
}
