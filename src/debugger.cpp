#include <asm_support.hpp>
#include <debugger.hpp>

Debugger Debugger::dbg;

// Add handler to the debugger.
void Debugger::AddHandler(size_t target, std::unique_ptr<Handler> handler) {
    handlers[target] = std::move(handler);
}

// Add tracer to the debugger.
void Debugger::AddTracer(size_t start, size_t end, std::unique_ptr<Tracer> tracer) {
    tracers.emplace_back(start, end, std::move(tracer));
}

// Set initial breakpoints.
void Debugger::SetInitialBreakPoint(size_t start, size_t end) {
    // add bp on specified address for handlers
    for (auto const&[addr, value] : handlers) {
        bps.Set(addr);
    }

    // set default start, end address based on text section
    auto[text_start, text_end] = Utils::GetTextSectionAddress();
    for (auto& pack : tracers) {
        // if start address is not specified
        if (pack.start == 0) {
            pack.start = text_start;
            pack.end = text_end;
        }

        bps.Set(pack.start);
        bps.Set(pack.end);
    }
}

// Set debugger.
void Debugger::SetDebugger(Debugger const& debugger) {
    dbg = debugger;
}

void Debugger::HandleSingleStep(PCONTEXT context) {
    // rewrite breakpoint
    if (dbg.last_bp) {
        dbg.bps.Set(dbg.last_bp);
        dbg.last_bp = 0;
    }

    // processing trace handler
    size_t iter = 0;
    for (auto& pack : dbg.tracers) {
        if (dbg.CheckTracer(iter)) {
            pack.tracer->HandleSingleStep(context, dbg.bps);
        }
        ++iter;
    }
}

bool Debugger::HandleBreakpoint(PCONTEXT context) {
    bool processed = false;
    auto recover = [&] {
        if (!processed) {
            dbg.last_bp = context->RegisterIp;
            dbg.bps.Recover(context->RegisterIp);
            processed = true;
        }
    };

    if (auto iter = dbg.handlers.find(context->Rip); iter != dbg.handlers.end()) {
        auto& handler = (*iter).second;
        handler->Handle(context);

        Utils::SetSingleStep(context);
        recover();
    }

    size_t iter = 0;
    for (auto& pack : dbg.tracers) {
        // start tracer
        if (pack.start == context->RegisterIp) {
            dbg.SetTracer(iter);
            recover();
        }
        // finish tracer
        if (pack.end == context->RegisterIp) {
            dbg.ReleaseTracer(iter);
            recover();
        }

        if (dbg.CheckTracer(iter)) {
            pack.tracer->HandleBreakpoint(context, dbg.bps);
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
long WINAPI Debugger::DebugHandler(PEXCEPTION_POINTERS exception) {
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
