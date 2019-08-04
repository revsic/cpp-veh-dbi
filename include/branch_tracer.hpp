#ifndef BRANCH_TRACER_HPP
#define BRANCH_TRACER_HPP

#include <Windows.h>

#include <fstream>
#include <string>

#include "tracer.hpp"

// Implementation of branch tracer.
struct BranchTracer : Tracer {
    // Constructor.
    BranchTracer(std::string filename, size_t start, size_t end, bool only_api = true);

    // Handle single step exception.
    void HandleSingleStep(PCONTEXT context) override;
    // Handle software breakpoint exception.
    void HandleBreakpoint(PCONTEXT context) override;

private:
    // Trace given context.
    void Trace(PCONTEXT context);
    // Wrtie log and set bp.
    bool LogAndBreak(size_t src, size_t called, size_t next);

    size_t start;
    size_t end;
    bool only_api;
    std::ofstream output;

    static bool init_sym;
};

#endif