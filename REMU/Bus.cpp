#include "Bus.h"

std::variant<uint64_t, Exception> Bus::Load(uint64_t addr, uint64_t size) {
    if ((CLINT_BASE <= addr) && (addr < (CLINT_BASE + CLINT_SIZE))) {
        return clint->load(addr, size);
    } else if ((PLIC_BASE <= addr) && (addr < (PLIC_BASE + PLIC_SIZE))) {
        return plic->load(addr, size);
    } else if ((UART_BASE <= addr) && (addr < (UART_BASE + UART_SIZE))) {
        return uart->load(addr, size);
    } else if (MEMORY_BASE <= addr) {
        return ram->load(addr, size);
    }
    return Exception::LoadAccessFault;
}

std::variant<uint64_t, Exception> Bus::Store(uint64_t addr, uint64_t size, uint64_t value) {
    if ((CLINT_BASE <= addr) && (addr < (CLINT_BASE + CLINT_SIZE))) {
        return clint->store(addr, size, value);
    } else if ((PLIC_BASE <= addr) && (addr < (PLIC_BASE + PLIC_SIZE))) {
        return plic->store(addr, size, value);
    } else if ((UART_BASE <= addr) && (addr < (UART_BASE + UART_SIZE))) {
        return uart->store(addr, size, value);
    } else if (MEMORY_BASE <= addr) {
        return ram->store(addr, size, value);
    }
    return Exception::StoreAMOAccessFault;
}