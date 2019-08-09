#include <asm_support.hpp>
#include <branch_tracer.hpp>
#include <dbi.hpp>

DBI DBI::dbi;

// Consturctor.
MultipleBTCallback::MultipleBTCallback(std::vector<std::unique_ptr<BTCallback>> callbacks) :
    callbacks(std::move(callbacks))
{
    // Do nothing
}

// Callback.
void MultipleBTCallback::run(BTInfo const& info, PCONTEXT context) {
    for (auto& callback : callbacks) {
        if (callback != nullptr) {
            callback->run(info, context);
        }
    }
}

// Consturctor.
DBI::DBI() : bps(), last_bp(0), trace_flag(0), handlers(), tracers() {
    // Do nothing
}

// Add handler to the DBI.
void DBI::AddHandler(size_t target, std::unique_ptr<Handler> handler) {
    handlers.emplace(target, std::move(handler));
}

// Add tracer to the DBI.
void DBI::AddTracer(size_t start, size_t end, std::unique_ptr<Tracer> tracer) {
    tracers.push_back({start, end, std::move(tracer)});
}

// Add BTCallback to the DBI.
void DBI::AddBTCallback(std::unique_ptr<BTCallback> callback) {
    btcallbacks.push_back(std::move(callback));
}

// Set initial breakpoints.
void DBI::SetInitialBreakPoint() {
    // add bp on specified address for handlers
    for (auto const&[addr, value] : handlers) {
        bps.Set(addr);
    }

    // set default start point as entrypoint
    size_t entrypoint = Utils::GetEntryPointAddress();
    // add bp on specified address for tracers
    for (auto& pack : tracers) {
        if (pack.start == 0) {
            pack.start = entrypoint;
        }

        bps.Set(pack.start);
        if (pack.end != 0) {
            bps.Set(pack.end);
        }
    }
}

// Run DBI.
void DBI::Run(DBI&& target) {
    // Add default branch tracer
    auto btcallbacks = std::make_unique<MultipleBTCallback>(std::move(target.btcallbacks));
    target.AddTracer(0, 0, std::make_unique<BranchTracer>(std::move(btcallbacks)));

    // set initial breakpoints
    target.SetInitialBreakPoint();
    // set breakpoint on entrypoint
    size_t entrypoint = Utils::GetEntryPointAddress();
    target.bps.Set(entrypoint);

    // set target as global context
    SetDBI(std::move(target));
    // add DBI veh handler
    AddVectoredExceptionHandler(1, DebugHandler);
}

// Set DBI.
void DBI::SetDBI(DBI&& target) {
    dbi = std::move(target);
}

// Handle single step exception.
void DBI::HandleSingleStep(PCONTEXT context) {
    // rewrite breakpoint
    if (dbi.last_bp) {
        dbi.bps.Set(dbi.last_bp);
        dbi.last_bp = 0;
    }

    // processing trace handler
    size_t iter = 0;
    for (auto& pack : dbi.tracers) {
        if (dbi.CheckTracer(iter)) {
            pack.tracer->HandleSingleStep(context, dbi.bps);
        }
        ++iter;
    }
}
// Handle breakpoint exception.
bool DBI::HandleBreakpoint(PCONTEXT context) {
    bool processed = false;
    auto recover = [&] {
        if (!processed) {
            dbi.last_bp = context->RegisterIp;
            dbi.bps.Recover(context->RegisterIp);
            processed = true;
        }
    };

    if (auto iter = dbi.handlers.find(context->Rip); iter != dbi.handlers.end()) {
        auto& handler = (*iter).second;
        handler->Handle(context);

        Utils::SetSingleStep(context);
        recover();
    }

    size_t iter = 0;
    for (auto& pack : dbi.tracers) {
        // start tracer
        if (pack.start == context->RegisterIp) {
            dbi.SetTracer(iter);
            recover();
        }
        // finish tracer
        if (pack.end == context->RegisterIp) {
            dbi.ReleaseTracer(iter);
            recover();
        }

        if (dbi.CheckTracer(iter)) {
            pack.tracer->HandleBreakpoint(context, dbi.bps);
            // for checking side effect
            if (*reinterpret_cast<BYTE*>(context->RegisterIp) != 0xCC) {
                processed = true;
            }
        }
        ++iter;
    }
    return processed;
}

// Real VEH handler.
long WINAPI DBI::DebugHandler(PEXCEPTION_POINTERS exception) {
    PEXCEPTION_RECORD record = exception->ExceptionRecord;
    PCONTEXT context = exception->ContextRecord;

    if (record->ExceptionCode == EXCEPTION_SINGLE_STEP) {
        HandleSingleStep(context);
        return EXCEPTION_CONTINUE_EXECUTION;
    } else if (record->ExceptionCode == EXCEPTION_BREAKPOINT) {
        bool processed = HandleBreakpoint(context);
        if (processed) {
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
