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
#include "Device.h"

class Plic : public Device {
public:
    Plic() : pending(0), senable(0), spriority(0), sclaim(0) { }

    std::variant<uint64_t, Exception> load(uint64_t addr, uint64_t size) override;
    std::variant<uint64_t, Exception> store(uint64_t addr, uint64_t size, uint64_t value) override;
private:
    uint64_t load32(uint64_t addr);
    void store32(uint64_t addr, uint64_t value);

    uint64_t pending;
    uint64_t senable;
    uint64_t spriority;
    uint64_t sclaim;
};