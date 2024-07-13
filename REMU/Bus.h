#pragma once
#include <variant>
#include <sstream>
#include "RAM.h"
#include "Plic.h"
#include "Clint.h"

#define MEMORY_BASE 0x80000000

#define PLIC_BASE 0xc000000
#define PLIC_SIZE 0x4000000

#define CLINT_BASE 0x2000000
#define CLINT_SIZE 0x10000

class Bus {
public:
	Bus(size_t size) : ram(std::make_shared<RAM>(size)), plic(std::make_shared<Plic>()), RamSize(size) {}

	std::shared_ptr<RAM> GetRam() { return ram; }
	std::shared_ptr<Plic> GetPlic() { return plic; }
	std::shared_ptr<Clint> GetClint() { return clint; }

	std::variant<uint64_t, Exception> Load(uint64_t addr, uint64_t size);
    std::variant<uint64_t, Exception> Store(uint64_t addr, uint64_t size, uint64_t value);

private:
	std::shared_ptr<RAM> ram;
	std::shared_ptr<Plic> plic;
	std::shared_ptr<Clint> clint;

	size_t RamSize;
};

