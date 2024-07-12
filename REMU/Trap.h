#pragma once
#include <cstdint>
#include <any>

enum class Exception : uint64_t {
    InstructionAddressMisaligned = 0,
    InstructionAccessFault = 1,
    IllegalInstruction = 2,
    Breakpoint = 3,
    LoadAddressMisaligned = 4,
    LoadAccessFault = 5,
    StoreAMOAddressMisaligned = 6,
    StoreAMOAccessFault = 7,
    EnvironmentCallFromUMode = 8,
    EnvironmentCallFromSMode = 9,
    EnvironmentCallFromMMode = 11,
    InstructionPageFault = 12,
    LoadPageFault = 13,
    StoreAMOPageFault = 15,
};

class Trap {
public:
    virtual uint64_t exception_code() const = 0;

    void take_trap(std::any an) const;
};

class ExceptionTrap : public Trap {
public:
    ExceptionTrap(Exception ex) : exception(ex) {}

    uint64_t exception_code() const override {
        return static_cast<uint64_t>(exception);
    }

private:
    Exception exception;
};

bool is_fatal(Exception ex);