#pragma once
#include <variant>
#include "RAM.h"
#include "Trap.h"

#define MEMORY_BASE 0x80000000

class Bus {
public:
	Bus(size_t size) : ram(std::make_shared<RAM>(size)) {}

	std::shared_ptr<RAM> GetRam() { return ram; }

	std::variant<uint64_t, Exception> Load(uint64_t addr, uint64_t size);

    std::variant<uint64_t, Exception> Store(uint64_t addr, uint64_t size, uint64_t value);

private:
	std::shared_ptr<RAM> ram;
};

