#include "Trap.h"
#include "CPU.h"

bool ExceptionTrap::is_fatal(Exception ex) {
    switch (ex) {
    case Exception::InstructionAddressMisaligned:
    case Exception::InstructionAccessFault:
    case Exception::LoadAccessFault:
    case Exception::StoreAMOAddressMisaligned:
    case Exception::StoreAMOAccessFault:
        return true;
    default:
        return false;
    }
}

void Trap::take_trap(std::any an) const {
    std::shared_ptr<CPU> cpu = std::any_cast<std::shared_ptr<CPU>>(an);
    uint64_t exception_pc = cpu->ProgramCounter - 4;
    Mode previous_mode = cpu->CurrentMode;

    uint64_t cause = exception_code();
    if ((previous_mode <= Mode::Supervisor) && (cpu->csrRead(MEDELEG) >> ((uint32_t)cause % 32)) & 1 != 0) {
        cpu->CurrentMode = Mode::Supervisor;

        cpu->ProgramCounter = cpu->csrRead(STVEC) & !1;

        cpu->csrWrite(SEPC, (exception_pc & !1));

        cpu->csrWrite(SCAUSE, cause);

        cpu->csrWrite(STVAL, 0);

        if (((cpu->csrRead(SSTATUS) >> 1) & 1) == 1) {
            cpu->csrWrite(SSTATUS, (cpu->csrRead(SSTATUS) | (1 << 5)));
        }
        else {
            cpu->csrWrite(SSTATUS, (cpu->csrRead(SSTATUS) & !(1 << 5)));
        }

        cpu->csrWrite(SSTATUS, (cpu->csrRead(SSTATUS) & !(1 << 1)));

        switch (previous_mode) {
        case Mode::User: cpu->csrWrite(SSTATUS, (cpu->csrRead(SSTATUS) & !(1 << 8))); break;
        default: cpu->csrWrite(SSTATUS, (cpu->csrRead(SSTATUS) | (1 << 8))); break;
        }
    }
    else {
        cpu->CurrentMode = Mode::Machine;

        cpu->ProgramCounter = cpu->csrRead(MTVEC) & !1;

        cpu->csrWrite(MEPC, exception_pc & !1);

        cpu->csrWrite(MCAUSE, cause);

        cpu->csrWrite(MTVAL, 0);

        if (((cpu->csrRead(MSTATUS) >> 3) & 1) == 1) {
            cpu->csrWrite(MSTATUS, (cpu->csrRead(MSTATUS) | (1 << 7)));
        }
        else {
            cpu->csrWrite(MSTATUS, (cpu->csrRead(MSTATUS) & !(1 << 7)));
        }

        cpu->csrWrite(MSTATUS, (cpu->csrRead(MSTATUS) & !(1 << 3)));

        cpu->csrWrite(MSTATUS, (cpu->csrRead(MSTATUS) & !(0b11 << 11)));
    }
}