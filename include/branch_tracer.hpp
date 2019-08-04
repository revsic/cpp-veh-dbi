#ifndef BRANCH_TRACER_HPP
#define BRANCH_TRACER_HPP

#include <Windows.h>

#include <fstream>
#include <mutex>
#include <string>

#include "tracer.hpp"
#include "utils.hpp"

// Implementation of branch tracer.
struct BranchTracer : Tracer {
    // Constructor.
    BranchTracer(std::string filename, size_t start, size_t end, bool only_api = true);

    // Handle single step exception.
    void HandleSingleStep(PCONTEXT context, Utils::SoftwareBP& bp) override;
    // Handle software breakpoint exception.
    void HandleBreakpoint(PCONTEXT context, Utils::SoftwareBP& bp) override;

private:
    // Trace given context.
    void Trace(PCONTEXT context, Utils::SoftwareBP& bp);
    // Write Log
    void Log(size_t src, size_t called);

    size_t start;
    size_t end;
    bool only_api;
    std::ofstream output;

    static std::once_flag init_sym;
};

#endif