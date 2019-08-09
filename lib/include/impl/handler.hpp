#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <Windows.h>

// Interface for debug event handler.
struct Handler {
    // Default virtual destructor.
    virtual ~Handler() = default;
    // Handle debug event.
    virtual void Handle(PCONTEXT context) = 0;
};

#endif