#pragma once
#include <variant>
#include "RAM.h"
#include "Plic.h"

#define MEMORY_BASE 0x80000000

#define PLIC_BASE 0xc000000
#define PLIC_SIZE 0x4000000

class Bus {
public:
	Bus(size_t size) : ram(std::make_shared<RAM>(size)), plic(std::make_shared<Plic>()) {}

	std::shared_ptr<RAM> GetRam() { return ram; }
	std::shared_ptr<Plic> GetPlic() { return plic; }

	std::variant<uint64_t, Exception> Load(uint64_t addr, uint64_t size);
    std::variant<uint64_t, Exception> Store(uint64_t addr, uint64_t size, uint64_t value);

private:
	std::shared_ptr<RAM> ram;
	std::shared_ptr<Plic> plic;
};

