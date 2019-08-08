#ifndef UTILS_HPP
#define UTILS_HPP

#include <Windows.h>

#include <string>
#include <unordered_map>

namespace Utils {
    // Get module name which includes given address.
    std::tuple<bool, std::string> GetModuleNameByAddr(size_t address);
    // Get symbol name of given address.
    std::string GetSymbolName(size_t address);

    // Set trap flag.
    void SetSingleStep(PCONTEXT context);

    struct SoftwareBP {
        // Set software breakpoint.
        void Set(size_t address);
        // Release breakpoint.
        bool Recover(size_t address);

        std::unordered_map<size_t, std::tuple<BYTE, bool>> bp;
    };

    // Get text section address.
    std::tuple<size_t, size_t> GetTextSectionAddress();
}

#endif