#include <asm_support.hpp>
#include <debugger.hpp>

Utils::SoftwareBP Debugger::bps;

size_t Debugger::last_bp = 0;
size_t Debugger::trace_flag = 0;

std::unordered_map<size_t, std::tuple<std::string, std::unique_ptr<Handler>>> Debugger::handlers;
std::vector<std::tuple<size_t, size_t, std::unique_ptr<Tracer>>> Debugger::tracers;

// Add handler to the debugger.
void Debugger::AddHandler(size_t target, std::string const& name, std::unique_ptr<Handler> handler) {
    handlers[target] = std::make_tuple(name, std::move(handler));
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
    for (auto&[start, end, handler] : tracers) {
        // if start address is not specified
        if (start == 0) {
            start = text_start;
            end = text_end;
        }

        bps.Set(start);
        bps.Set(end);
    }
}

// Real VEH handler.
long WINAPI Debugger::veh_handler(PEXCEPTION_POINTERS exception) {
    PEXCEPTION_RECORD record = exception->ExceptionRecord;
    PCONTEXT context = exception->ContextRecord;

    if (record->ExceptionCode == EXCEPTION_SINGLE_STEP) {
        // rewrite breakpoint
        if (last_bp) {
            bps.Set(last_bp);
            last_bp = 0;
        }

        // processing trace handler
        size_t iter = 0;
        for (auto const&[start, end, handler] : tracers) {
            if ((trace_flag >> iter) & 1) {
                handler->HandleSingleStep(context, bps);
            }
            ++iter;
        }

        return EXCEPTION_CONTINUE_EXECUTION;
    } else if (record->ExceptionCode == EXCEPTION_BREAKPOINT) {
        bool processed = false;
        auto recover = [&] {
            if (!processed) {
                last_bp = context->RegisterIp;
                bps.Recover(context->RegisterIp);
                processed = true;
            }
        };

        if (auto iter = handlers.find(context->Rip); iter != handlers.end()) {
            auto&[name, handler] = (*iter).second;
            handler->Handle(context);

            Utils::SetSingleStep(context);
            recover();
        }

        size_t iter = 0;
        for (auto const&[start, end, handler] : tracers) {
            // start tracer
            if (start == context->RegisterIp) {
                trace_flag |= (1 << iter);
                recover();
            }
            // finish tracer
            if (end == context->RegisterIp) {
                trace_flag ^= (1 << iter);
                recover();
            }

            if ((trace_flag >> iter) & 1) {
                handler->HandleBreakpoint(context, bps);

                // for checking side effect
                if (*reinterpret_cast<BYTE*>(context->RegisterIp) != 0xCC) {
                    processed = true;
                }
            }
            ++iter;
        }

        if (processed) {
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
        return EXCEPTION_CONTINUE_SEARCH;
}