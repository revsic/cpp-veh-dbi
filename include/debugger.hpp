#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <Windows.h>

namespace Debugger {
    // VEH based dynamic binary instrumentation.
    long WINAPI veh_handler(PEXCEPTION_POINTERS exception);
}

#endif