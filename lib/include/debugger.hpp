#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <Windows.h>

#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "handler.hpp"
#include "tracer.hpp"
#include "utils.hpp"

// VEH based debugger.
struct Debugger {
    // Constructor.
    Debugger();

    // Add handler to the debugger.
    void AddHandler(size_t target, std::unique_ptr<Handler> handler);
    // Add tracer to the debugger.
    void AddTracer(size_t start, size_t end, std::unique_ptr<Tracer> tracer);

    // Set initial breakpoints.
    void SetInitialBreakPoint();

    // Run debugger.
    static void Run(Debugger&& debugger);
    // Set debugger.
    static void SetDebugger(Debugger&& debugger);
    // Real VEH handler.
    static long WINAPI DebugHandler(PEXCEPTION_POINTERS exception);

private:
    static Debugger dbg;

    // Handle single step exception.
    static void HandleSingleStep(PCONTEXT context);
    // Handle breakpoint exception.
    static bool HandleBreakpoint(PCONTEXT context);

    // Set tracer flag.
    inline void SetTracer(size_t idx) {
        trace_flag |= (1LL << idx);
    }
    // Release tracer flag.
    inline void ReleaseTracer(size_t idx) {
        trace_flag ^= (1LL << idx);
    }
    // Check tracer set.
    inline bool CheckTracer(size_t idx) const {
        return (dbg.trace_flag >> idx) & 1;
    }

    Utils::SoftwareBP bps;
    
    size_t last_bp;
    size_t trace_flag;

    struct TracerPack {
        size_t start;
        size_t end;
        std::unique_ptr<Tracer> tracer;
    };

    std::unordered_map<size_t, std::unique_ptr<Handler>> handlers;
    std::vector<TracerPack> tracers;
};

#endif