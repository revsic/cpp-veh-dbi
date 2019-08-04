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
    // Add handler to the debugger.
    static void AddHandler(size_t target, std::string const& name, std::unique_ptr<Handler> handler);
    // Add tracer to the debugger.
    static void AddTracer(size_t start, size_t end, std::unique_ptr<Tracer> tracer);

    // Set initial breakpoints.
    static void SetInitialBreakPoint(size_t start, size_t end);

    // Real VEH handler.
    static long WINAPI veh_handler(PEXCEPTION_POINTERS exception);

private:
    static Utils::SoftwareBP bps;
    
    static size_t last_bp;
    static size_t trace_flag;

    static std::unordered_map<size_t, std::tuple<std::string, std::unique_ptr<Handler>>> handlers;
    static std::vector<std::tuple<size_t, size_t, std::unique_ptr<Tracer>>> tracers;
};

#endif