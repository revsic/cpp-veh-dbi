#include <DbgHelp.h>

#include <asm_support.hpp>
#include <branch_tracer.hpp>

std::once_flag BranchTracer::init_sym;
Utils::SoftwareBP BranchTracer::global_bp;

// Constructor.
BranchTracer::BranchTracer(std::string filename, size_t start, size_t end, bool only_api) :
    start(start), end(end), only_api(only_api), output(filename)
{
    std::call_once(init_sym, []{ SymInitialize(GetCurrentProcess(), NULL, TRUE); });
}

// Handle single step exception.
void BranchTracer::HandleSingleStep(PCONTEXT context) {
    Trace(context);
}

// Handle software breakpoint exception.
void BranchTracer::HandleBreakpoint(PCONTEXT context) {
    global_bp.Recover(context->RegisterIp);
    Trace(context);
}

// Trace given context.
void BranchTracer::Trace(PCONTEXT context) {
    bool bp_set = false;
    BYTE* opc = reinterpret_cast<BYTE*>(context->RegisterIp);

    auto jmp_call = [](BYTE* opc) {
        if (opc[0] == 0xFF) {
            BYTE reg = (opc[1] >> 0x3) & 0x7;
            // Binary 010(near call) or 011(far call) 
            //        100(near jmp)  or 101(far jmp)
            return reg >= 2 && reg <= 5;
        }
        return false;
    };

    // instruction call
    if (opc[0] == 0xE8) {
        size_t called = context->RegisterIp + *reinterpret_cast<long*>(opc + 1) + 5;
        BYTE* called_opc = reinterpret_cast<BYTE*>(called);

        // if instruction jump to windows api
        if (jmp_call(called_opc)) {
            auto[called_next, retn] = ASMSupport::GetBranchingAddress(opc, context);
            Log(context->RegisterIp, called_next);

            if (!(start <= called_next && called_next <= end)) {
                global_bp.Set(retn);
                bp_set = true;
            }
        } else if (!only_api) {
            Log(context->RegisterIp, called);
        }
    } else if (jmp_call(opc)) {
        auto[called, retn] = ASMSupport::GetBranchingAddress(opc, context);
        Log(context->RegisterIp, called);

        if (!(start <= called && called <= end)) {
            global_bp.Set(retn);
            bp_set = true;
        }
    }

    if (!bp_set) {
        Utils::SetSingleStep(context);
    }
}

// Write log.
void BranchTracer::Log(size_t src, size_t called) {
    void* src_ptr = reinterpret_cast<void*>(src);
    void* called_ptr = reinterpret_cast<void*>(called);

    if (!(start <= src && src <= end)) {
        auto[load_module, module_name] = Utils::GetModuleNameByAddr(called);
        if (load_module) {
            std::string symbol_name = Utils::GetSymbolName(called);
            output << '+' << src_ptr << ',' << called_ptr << ',' << module_name << ',' << symbol_name << "\r\n";
        } else {
            output << '+' << src_ptr << ',' << called_ptr << ",,\r\n";
        }
    } else if (!only_api) {
        output << '+' << src_ptr << ',' << called_ptr << ",,\r\n";
    }
}
