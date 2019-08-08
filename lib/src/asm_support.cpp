#include <asm_support.hpp>

namespace ASMSupport {
    // RnM bit
    enum class RnM {
        AX = 0,
        CX = 1,
        DX = 2,
        BX = 3,
        SP = 4,
        BP = 5,
        SI = 6,
        DI = 7,
    };

    // Disassemble and return branching address and return address.
    std::tuple<size_t, size_t> GetBranchingAddress(BYTE* opc, PCONTEXT context) {
        BYTE mod = opc[1] >> 0x6; // high 2bits
        BYTE reg = (opc[1] >> 0x3) & 0x7; // mid 3bits
        RnM rnm = static_cast<RnM>(opc[1] & 0x7); // low 3bits

        size_t called = 0;
        size_t register_ip = reinterpret_cast<size_t>(opc);

        switch (rnm) {
        case RnM::AX: 
            called = context->RegisterAx; 
            break;
        case RnM::CX:
            called = context->RegisterCx;
            break;
        case RnM::DX:
            called = context->RegisterDx;
            break;
        case RnM::BX:
            called = context->RegisterBx;
            break;
        case RnM::SP:
            if (mod != 0x3) { // binary 11
                auto[parsed_opc, parsed_called] = SIBParser(opc, context);

                opc = parsed_opc;
                called = parsed_called;
            } else {
                called = context->RegisterSp;
            }
            break;
        case RnM::BP:
            if (mod == 0x00) {
#ifdef _WIN64
                called = register_ip + *reinterpret_cast<long*>(opc + 2) + 6;
#else
                called = *reinterpret_cast<long*>(opc + 2);
#endif
                opc += 4;
            } else {
                called = context->RegisterBp;
            }
            break;
        case RnM::SI:
            called = context->RegisterSi;
            break;
        case RnM::DI:
            called = context->RegisterDi;
            break;
        }

        if (mod == 0x1) { //binary 01
            called += static_cast<char>(opc[2]);
            ++opc;
        } else if (mod == 0x2) { //binary 10
            called += *reinterpret_cast<long*>(opc + 2);
            opc += 4;
        }

        if (mod != 0x3) { //binary 11
            called = *reinterpret_cast<size_t*>(called);
        }

        size_t retn = 0;
        if (reg == 2 || reg == 3) { //binary 010 (near call) , 011 (far call)
            retn = reinterpret_cast<size_t>(opc + 2);
        } else if (reg == 4 || reg == 5) { //binary 100 (near jmp) , 101 (far jmp)
            retn = *reinterpret_cast<size_t*>(context->RegisterSp);
        }

        return std::make_tuple(called, retn);
    }

    // Parse SIB and return next opcode pointer and branching address.
    std::tuple<BYTE*, size_t> SIBParser(BYTE* opc, PCONTEXT context) {
        BYTE sib = opc[2];
        BYTE scale = sib >> 0x6; // high 2bits
        RnM index = static_cast<RnM>((sib >> 0x3) & 0x7); // mid 3bits
        RnM base = static_cast<RnM>(sib & 0x7); // low 3bits

        size_t called = 0;
        switch (index) {
        case RnM::AX: called = context->RegisterAx; break;
        case RnM::CX: called = context->RegisterCx; break;
        case RnM::DX: called = context->RegisterDx; break;
        case RnM::BX: called = context->RegisterBx; break;
        case RnM::SP: break; //None
        case RnM::BP: called = context->RegisterBp; break;
        case RnM::SI: called = context->RegisterSi; break;
        case RnM::DI: called = context->RegisterDi; break;
        }

        called = called * (1LL << scale);

        switch (base) {
        case RnM::AX: called += static_cast<long>(context->RegisterAx); break;
        case RnM::CX: called += static_cast<long>(context->RegisterCx); break;
        case RnM::DX: called += static_cast<long>(context->RegisterDx); break;
        case RnM::BX: called += static_cast<long>(context->RegisterBx); break;
        case RnM::SP: called += static_cast<long>(context->RegisterSp); break;
        case RnM::BP: {
            BYTE Mod = opc[1] >> 6;
            switch (Mod) {
            case 0: //binary 00
                called += *reinterpret_cast<long*>(opc + 3);
                opc += 4;
                break;
            case 1: //binary 01
                called += static_cast<char>(opc[3]) + context->RegisterBp;
                ++opc;
                break;
            case 2: //binary 10
                called += *reinterpret_cast<long*>(opc + 3) + context->RegisterBp;
                opc += 4;
                break;
            }
            break;
        }
        case RnM::SI: called += static_cast<long>(context->RegisterSi); break;
        case RnM::DI: called += static_cast<long>(context->RegisterDi); break;
        }

        return std::make_tuple(opc + 1, called);
    }
}