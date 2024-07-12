#include "Bus.h"

std::variant<uint64_t, Exception> Bus::Load(uint64_t addr, uint64_t size) {
    if (MEMORY_BASE <= addr) {
        return ram->Load(addr, size);
    }
    return Exception::LoadAccessFault;
}

std::variant<uint64_t, Exception> Bus::Store(uint64_t addr, uint64_t size, uint64_t value) {
    if (MEMORY_BASE <= addr) {
        return ram->Load(addr, size);
    }
    return Exception::StoreAMOAccessFault;
}