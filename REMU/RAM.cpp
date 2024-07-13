#include "RAM.h"

std::variant<uint64_t, Exception> RAM::load(uint64_t addr, uint64_t size) {
    switch (size) {
    case 8:  return MemoryLoad8(addr);  break;
    case 16: return MemoryLoad16(addr); break;
    case 32: return MemoryLoad32(addr); break;
    case 64: return MemoryLoad64(addr); break;
    default: break;
    }
    return Exception::LoadAccessFault;
}

std::variant<uint64_t, Exception> RAM::store(uint64_t addr, uint64_t size, uint64_t value) {
    switch (size) {
    case 8:  MemoryStore8(addr, value);  break;
    case 16: MemoryStore16(addr, value); break;
    case 32: MemoryStore32(addr, value); break;
    case 64: MemoryStore64(addr, value); break;
    default: return Exception::StoreAMOAccessFault;
    }
    return (uint64_t)0;
}

uint64_t RAM::MemoryLoad8(uint64_t addr) {
    return (uint64_t)(*Memory)[addr - MEMORY_BASE];
}

uint64_t RAM::MemoryLoad16(uint64_t addr) {
    return (uint64_t)(*Memory)[addr - MEMORY_BASE]
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 1] << 8;
}

uint64_t RAM::MemoryLoad32(uint64_t addr) {
    return (uint64_t)(*Memory)[addr - MEMORY_BASE]
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 1] << 8
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 2] << 16
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 3] << 24;
}

uint64_t RAM::MemoryLoad64(uint64_t addr) {
    return (uint64_t)(*Memory)[addr - MEMORY_BASE]
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 1] << 8
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 2] << 16
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 3] << 24
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 4] << 32
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 5] << 40
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 6] << 48
        | (uint64_t)(*Memory)[addr - MEMORY_BASE + 7] << 56;
}

void RAM::MemoryStore8(uint64_t addr, uint64_t value) {
    (*Memory)[addr - MEMORY_BASE] = (uint8_t)(value & 0xff);
}

void RAM::MemoryStore16(uint64_t addr, uint64_t value) {
    (*Memory)[addr - MEMORY_BASE] = (uint8_t)(value & 0xff);
    (*Memory)[addr - MEMORY_BASE + 1] = (uint8_t)((value >> 8) & 0xff);
}

void RAM::MemoryStore32(uint64_t addr, uint64_t value) {
    (*Memory)[addr - MEMORY_BASE] = (uint8_t)(value & 0xff);
    (*Memory)[addr - MEMORY_BASE + 1] = (uint8_t)((value >> 8) & 0xff);
    (*Memory)[addr - MEMORY_BASE + 2] = (uint8_t)((value >> 16) & 0xff);
    (*Memory)[addr - MEMORY_BASE + 3] = (uint8_t)((value >> 24) & 0xff);
}

void RAM::MemoryStore64(uint64_t addr, uint64_t value) {
    (*Memory)[addr - MEMORY_BASE] = (uint8_t)(value & 0xff);
    (*Memory)[addr - MEMORY_BASE + 1] = (uint8_t)((value >> 8) & 0xff);
    (*Memory)[addr - MEMORY_BASE + 2] = (uint8_t)((value >> 16) & 0xff);
    (*Memory)[addr - MEMORY_BASE + 3] = (uint8_t)((value >> 24) & 0xff);
    (*Memory)[addr - MEMORY_BASE + 4] = (uint8_t)((value >> 32) & 0xff);
    (*Memory)[addr - MEMORY_BASE + 5] = (uint8_t)((value >> 40) & 0xff);
    (*Memory)[addr - MEMORY_BASE + 6] = (uint8_t)((value >> 48) & 0xff);
    (*Memory)[addr - MEMORY_BASE + 7] = (uint8_t)((value >> 56) & 0xff);
}

std::shared_ptr<std::vector<uint8_t>> RAM::ReturnMemory() {
    return Memory;
}

RAM::RAM(size_t size) {
    MemorySize = 1024 * 1024 * size; // (size) MiB
    Memory = std::make_shared<std::vector<uint8_t>>();
    (*Memory).resize(MemorySize);
}