#pragma once
#define MEMORY_BASE 0x80000000
#include <memory>
#include <vector>

class RAM {
public:
	RAM(size_t size);

	std::shared_ptr<std::vector<uint8_t>> ReturnMemory();

    uint64_t MemoryLoad8(uint64_t addr);
    uint64_t MemoryLoad16(uint64_t addr);
    uint64_t MemoryLoad32(uint64_t addr);
    uint64_t MemoryLoad64(uint64_t addr);

    void MemoryStore8(uint64_t addr, uint64_t value);
    void MemoryStore16(uint64_t addr, uint64_t value);
    void MemoryStore32(uint64_t addr, uint64_t value);
    void MemoryStore64(uint64_t addr, uint64_t value);

private:
	std::shared_ptr<std::vector<uint8_t>> Memory;
    size_t MemorySize;
};