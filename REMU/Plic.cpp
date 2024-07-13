#include "Plic.h"

uint64_t Plic::load32(uint64_t addr) {
    switch (addr) {
    case PLIC_PENDING: return pending; break;
    case PLIC_SENABLE: return senable; break;
    case PLIC_SPRIORITY: return spriority; break;
    case PLIC_SCLAIM: return sclaim; break;
    default: return 0; break;
    }
}

void Plic::store32(uint64_t addr, uint64_t value) {
    switch (addr) {
    case PLIC_PENDING: pending = value; break;
    case PLIC_SENABLE: senable = value; break;
    case PLIC_SPRIORITY:spriority = value; break;
    case PLIC_SCLAIM: sclaim = value; break;
    default: break;
    }
}

std::variant<uint64_t, Exception> Plic::store(uint64_t addr, uint64_t size, uint64_t value) {
    if (size == 32) {
        store32(addr, value);
    }
    else {
        return Exception::StoreAMOAccessFault;
    }
}

std::variant<uint64_t, Exception> Plic::load(uint64_t addr, uint64_t size) {
    if (size == 32) {
        return load32(addr);
    }
    else {
        return Exception::LoadAccessFault;
    }
}