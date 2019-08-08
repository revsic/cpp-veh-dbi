#ifndef ASM_SUPPORT_HPP
#define ASM_SUPPORT_HPP

#include <Windows.h>

#include <tuple>

namespace ASMSupport {
    // Disassemble and return branching address and return address.
    std::tuple<size_t, size_t> GetBranchingAddress(BYTE* opc, PCONTEXT context);
    // Parse SIB and return next opcode pointer and branching address.
    std::tuple<BYTE*, size_t> SIBParser(BYTE* opc, PCONTEXT context);
}

// if x64 environment
#ifdef _WIN64
// declare register with rax-family
#define RegisterAx Rax
#define RegisterCx Rcx
#define RegisterDx Rdx
#define RegisterBx Rbx
#define RegisterSp Rsp
#define RegisterBp Rbp
#define RegisterSi Rsi
#define RegisterDi Rdi
#define RegisterIp Rip
// if x86 environment
#else
// declare register with eax-familty
#define RegisterAx Eax
#define RegisterCx Ecx
#define RegisterDx Edx
#define RegisterBx Ebx
#define RegisterSp Esp
#define RegisterBp Ebp
#define RegisterSi Esi
#define RegisterDi Edi
#define RegisterIp Eip
#endif

#endif