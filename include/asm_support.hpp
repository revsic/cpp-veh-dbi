#ifndef ASM_SUPPORT_HPP
#define ASM_SUPPORT_HPP

#include <Windows.h>

#include <tuple>

namespace ASMSupport {
    // Disassemble and return branching address.
    size_t GetBranchingAddress(BYTE* opc, PCONTEXT context);
    // Parse SIB and return next opcode pointer and branching address.
    std::tuple<BYTE*, size_t> SIBParser(BYTE* opc, PCONTEXT context);
}

#endif