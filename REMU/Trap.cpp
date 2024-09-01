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

void Trap::take_trap(std::any cpu) {
    take_trap_helper(cpu, false);
}

void Trap::take_trap_helper(std::any an, bool is_interrupt) const {
    std::shared_ptr<CPU> cpu = std::any_cast<std::shared_ptr<CPU>>(an);
    uint64_t exception_pc = cpu->ProgramCounter - 4;
    Mode previous_mode = cpu->CurrentMode;

    uint64_t cause = exception_code();
    if (is_interrupt) {
        cause = (1 << 63) | cause;
    }
    if ((previous_mode <= Mode::Supervisor) && (cpu->csrRead(MEDELEG) >> static_cast<uint32_t>(cause)) & 1 != 0) {
        cpu->CurrentMode = Mode::Supervisor;

        if (is_interrupt) {
            uint64_t stvec = cpu->csrRead(STVEC);
            uint64_t vector;
            switch (stvec & 1) {
            case 1: vector = (4 * cause); break;
            default: vector = 0; break;
			}
            cpu->ProgramCounter = (stvec & !1) + vector;
        }
        else {
            cpu->ProgramCounter = cpu->csrRead(STVEC) & !1;
        }

        cpu->csrWrite(SEPC, (exception_pc & !1));

        cpu->csrWrite(SCAUSE, cause);

        cpu->csrWrite(STVAL, 0);

        if (((cpu->csrRead(SSTATUS) >> 1) & 1) == 1) {
            cpu->csrWrite(SSTATUS, (cpu->csrRead(SSTATUS) | (1 << 5)));
        } else {
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

        if (is_interrupt) {
            uint64_t mtvec = cpu->csrRead(MTVEC);
            uint64_t vector;
            switch (mtvec & 1) {
            case 1: vector = (4 * cause); break;
            default: vector = 0; break;
			}
            cpu->ProgramCounter = (mtvec & !1) + vector;
        }
        else {
            cpu->ProgramCounter = cpu->csrRead(MTVEC) & !1;
        }

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

uint64_t InterruptTrap::exception_code() const {
    switch (interrupt) {
    case Interrupt::UserSoftwareInterrupt: return 0; break;
    case Interrupt::SupervisorSoftwareInterrupt: return 1; break;
    case Interrupt::MachineSoftwareInterrupt: return 3; break;
    case Interrupt::UserTimerInterrupt: return 4; break;
    case Interrupt::SupervisorTimerInterrupt: return 5; break;
    case Interrupt::MachineTimerInterrupt: return 7; break;
    case Interrupt::UserExternalInterrupt: return 8; break;
    case Interrupt::SupervisorExternalInterrupt: return 9; break;
    case Interrupt::MachineExternalInterrupt: return 11; break;
    }
}

void InterruptTrap::take_trap(std::any cpu) {
    take_trap_helper(cpu, true);
}