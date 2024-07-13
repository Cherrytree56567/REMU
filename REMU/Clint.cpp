#include "Clint.h"

std::variant<uint64_t, Exception> Clint::load(uint64_t addr, uint64_t size) {
    if (size == 32) {
        return load32(addr);
    } else {
        return Exception::LoadAccessFault;
    }
}

std::variant<uint64_t, Exception> Clint::store(uint64_t addr, uint64_t size, uint64_t value) {
    if (size == 32) {
        store32(addr, value);
    } else {
        return Exception::StoreAMOAccessFault;
    }
}

uint64_t Clint::load32(uint64_t addr) {
    switch (addr) {
    case CLINT_MTIMECMP: return mtimecmp; break;
    case CLINT_MTIME: return mtime; break;
    default: return 0; break;
    }
}

void Clint::store32(uint64_t addr, uint64_t value) {
    switch (addr) {
    case CLINT_MTIMECMP: mtimecmp = value; break;
    case CLINT_MTIME: mtime = value; break;
    default: break;
    }
}