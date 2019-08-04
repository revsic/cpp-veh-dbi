#ifndef UTILS_HPP
#define UTILS_HPP

#include <Windows.h>

#include <string>

namespace Utils {
    // Get module name which includes given address.
    std::string GetModuleNameByAddr(size_t address);
    // Get symbol name of given address.
    std::string GetSymbolName(size_t called);

    // Set trap flag.
    void SetSingleStep(PCONTEXT context);
    // Set software breakpoint.
    bool SetBreakPoint(size_t address);
    // Release breakpoint.
    bool RecoverBreakpoint(size_t address);
    
    // Get text section address.
    std::tuple<size_t, size_t> GetTextSectionAddress();
}

#endif