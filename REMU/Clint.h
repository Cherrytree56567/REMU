#pragma once
#define CLINT_BASE 0x2000000
#define CLINT_SIZE 0x10000
#define CLINT_MTIMECMP CLINT_BASE + 0x4000
#define CLINT_MTIME CLINT_BASE + 0xbff8
#include "Device.h"

class Clint : public Device {
public:
    Clint() : mtime(0), mtimecmp(0) { }

    std::variant<uint64_t, Exception> load(uint64_t addr, uint64_t size) override;
    std::variant<uint64_t, Exception> store(uint64_t addr, uint64_t size, uint64_t value) override;

private:
    uint64_t load64(uint64_t addr);

    void store64(uint64_t addr, uint64_t value);

    uint64_t mtime;
    uint64_t mtimecmp;
};