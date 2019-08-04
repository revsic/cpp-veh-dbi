#ifndef UTILS_HPP
#define UTILS_HPP

#include <Windows.h>

#include <string>

namespace Utils {
    // Get module name which includes given address.
    std::string GetModuleNameByAddr(DWORD address);
    // Get symbol name of given address.
    std::string GetSymbolName(DWORD called);

    // Set trap flag.
    void SetSingleStep(PCONTEXT context);
    // Set software breakpoint.
    bool SetBreakPoint(DWORD address);
    // Release breakpoint.
    bool RecoverBreakpoint(DWORD address);
    
    // Get text section address.
    std::tuple<DWORD, DWORD> GetTextSectionAddress();
}

#endif