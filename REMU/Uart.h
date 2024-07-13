#pragma once
#define UART_BASE 0x10000000
#define UART_SIZE 0x100
#define UART_RHR UART_BASE + 0
#define UART_THR UART_BASE + 0
#define UART_LCR UART_BASE + 3
#define UART_LSR UART_BASE + 5
#define UART_LSR_RX 1
#define UART_LSR_TX 1 << 5
#include <cstdint>
#include <variant>
#include <array>
#include <tuple>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <iostream>
#include <thread>
#include "Trap.h"
#include "Device.h"

class Uart : public Device {
public:
    Uart();

    std::variant<uint64_t, Exception> load(uint64_t addr, uint64_t size) override;
    std::variant<uint64_t, Exception> store(uint64_t addr, uint64_t size, uint64_t value) override;
private:
    uint64_t load8(uint64_t addr);
    void store8(uint64_t addr, uint64_t value);

    std::shared_ptr<std::tuple<std::mutex, std::condition_variable, std::array<uint8_t, UART_SIZE>>> uart;
    std::shared_ptr<std::atomic_bool> interrupting;
};