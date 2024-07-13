#pragma once
#define PLIC_BASE 0xc000000
#define PLIC_SIZE 0x4000000

#define PLIC_PENDING 0xc000000 + 0x1000
#define PLIC_SENABLE 0xc000000 + 0x2080
#define PLIC_SPRIORITY 0xc000000 + 0x201000
#define PLIC_SCLAIM 0xc000000 + 0x201004
#include <cstdint>
#include <variant>
#include "Trap.h"

class Device {
public:
    virtual std::variant<uint64_t, Exception> load(uint64_t addr, uint64_t size) = 0;
    virtual std::variant<uint64_t, Exception> store(uint64_t addr, uint64_t size, uint64_t value) = 0;
};

class Plic : public Device {
public:
    Plic() {
        pending = 0;
        senable = 0;
        spriority = 0;
        sclaim = 0;
    }

    std::variant<uint64_t, Exception> load(uint64_t addr, uint64_t size) override {
        if (size == 32) {
            return load32(addr);
        }
        else {
            return Exception::LoadAccessFault;
        }
    }

    std::variant<uint64_t, Exception> store(uint64_t addr, uint64_t size, uint64_t value) override {
        if (size == 32) {
            store32(addr, value);
        }
        else {
            return Exception::StoreAMOAccessFault;
        }
    }

    uint64_t load32(uint64_t addr) {
        switch (addr) {
        case PLIC_PENDING: return pending; break;
        case PLIC_SENABLE: return senable; break;
        case PLIC_SPRIORITY: return spriority; break;
        case PLIC_SCLAIM: return sclaim; break;
        default: return 0; break;
        }
    }

    void store32(uint64_t addr, uint64_t value) {
        switch (addr) {
        case PLIC_PENDING: pending = value; break;
        case PLIC_SENABLE: senable = value; break;
        case PLIC_SPRIORITY:spriority = value; break;
        case PLIC_SCLAIM: sclaim = value; break;
        default: break;
        }
    }
private:
    uint64_t pending;
    uint64_t senable;
    uint64_t spriority;
    uint64_t sclaim;
};