#pragma once
#include <variant>
#include <cstdint>
#include "Trap.h"

class Device {
public:
    virtual std::variant<uint64_t, Exception> load(uint64_t addr, uint64_t size) = 0;
    virtual std::variant<uint64_t, Exception> store(uint64_t addr, uint64_t size, uint64_t value) = 0;
};
