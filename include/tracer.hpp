#ifndef TRACER_HPP
#define TRACER_HPP

#include <Windows.h>

#include "utils.hpp"

// Interface for code trace handler.
struct Tracer {
    // Default virtual destructor.
    virtual ~Tracer() = default;
    // Handle single step exception.
    virtual void HandleSingleStep(PCONTEXT context, Utils::SoftwareBP& bp) = 0;
    // Handle software breakpoint exception.
    virtual void HandleBreakpoint(PCONTEXT context, Utils::SoftwareBP& bp) = 0;
};

#endif