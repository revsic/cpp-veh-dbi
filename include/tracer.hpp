#ifndef TRACER_HPP
#define TRACER_HPP

#include <Windows.h>

// Interface for code trace handler.
struct Tracer {
    // Default virtual destructor.
    virtual ~Tracer() = default;
    // Handle single step exception.
    virtual void HandleSingleStep(PCONTEXT context) = 0;
    // Handle software breakpoint exception.
    virtual void HandleBreakpoint(PCONTEXT context) = 0;
};

#endif