#ifndef BRANCH_TRACER_HPP
#define BRANCH_TRACER_HPP

#include <Windows.h>

#include <mutex>

#include "tracer.hpp"
#include "utils.hpp"

// Branch data from branch tracer.
struct BTInfo {
    size_t source = 0;
    size_t called = 0;
    size_t retn = 0;
    bool e8_branch = false;
    bool ff_branch = false;
};

// Callback for branch tracer.
struct BTCallback {
    // Default destructor.
    virtual ~BTCallback() = default;
    // Callback.
    virtual void run(BTInfo const& info, PCONTEXT context) = 0;
};

// Implementation of branch tracer.
struct BranchTracer : Tracer {
    // Constructor.
    BranchTracer(size_t start, size_t end, std::unique_ptr<BTCallback> callback = nullptr);
    // Text section based tracer.
    BranchTracer(std::unique_ptr<BTCallback> callback = nullptr);

    // Handle single step exception.
    void HandleSingleStep(PCONTEXT context, Utils::SoftwareBP& bp) override;
    // Handle software breakpoint exception.
    void HandleBreakpoint(PCONTEXT context, Utils::SoftwareBP& bp) override;

private:
    // Trace given context.
    void Trace(PCONTEXT context, Utils::SoftwareBP& bp);

    size_t start;
    size_t end;
    std::unique_ptr<BTCallback> callback;

    static std::once_flag init_sym;
};

#endif