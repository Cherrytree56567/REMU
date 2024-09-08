#ifdef LINUX
#include <unistd.h>
#include <fcntl.h>
#else
#include <windows.h>
#endif
#include "Uart.h"

#ifdef LINUX
Uart::Uart() {
    uart = std::make_shared<std::tuple<std::mutex, std::condition_variable, std::array<uint8_t, UART_SIZE>>>();
    interrupting = std::make_shared<std::atomic_bool>(false);

    {
        std::lock_guard<std::mutex> lock(std::get<0>((*uart)));
        std::get<2>((*uart))[UART_LSR - UART_BASE] |= UART_LSR_TX;
    }

    std::thread([this]() {
        std::array<uint8_t, 1> byte;
        int fd = STDIN_FILENO; // Use file descriptor for standard input
        while (true) {
            ssize_t bytesRead = read(fd, byte.data(), byte.size());
            if (bytesRead > 0) {
                std::unique_lock<std::mutex> lock(std::get<0>((*uart)));
                // Wait for the thread to start up.
                while ((std::get<2>((*uart))[UART_LSR - UART_BASE] & UART_LSR_RX) == 1) {
                    std::get<1>((*uart)).wait(lock);
                }
                std::get<2>((*uart))[0] = byte[0];
                interrupting->store(true, std::memory_order_release);
                // Data has been received.
                std::get<2>((*uart))[UART_LSR - UART_BASE] |= UART_LSR_RX;
            } else {
                std::cerr << "Error reading from stdin" << std::endl;
            }
        }
    }).detach();
}
#else
Uart::Uart() {
    uart = std::make_shared<std::tuple<std::mutex, std::condition_variable, std::array<uint8_t, UART_SIZE>>>();
    interrupting = std::make_shared<std::atomic_bool>(false);

    {
        std::lock_guard<std::mutex> lock(std::get<0>((*uart)));
        std::get<2>((*uart))[UART_LSR - UART_BASE] |= UART_LSR_TX;
    }

    std::thread([this]() {
        std::array<uint32_t, 1> byte;
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD bytesRead;
        while (true) {
            if (ReadFile(hStdin, byte.data(), byte.size(), &bytesRead, NULL) && bytesRead > 0) {
                std::unique_lock<std::mutex> lock(std::get<0>((*uart)));
                // Wait for the thread to start up.
                while ((std::get<2>((*uart))[UART_LSR - UART_BASE] & UART_LSR_RX) == 1) {
                    std::get<1>((*uart)).wait(lock);
                }
                std::get<2>((*uart))[0] = byte[0];
                interrupting->store(true, std::memory_order_release);
                // Data has been received.
                std:get<2>((*uart))[UART_LSR - UART_BASE] |= UART_LSR_RX;
            }
            else {
                std::cerr << "Error reading from stdin" << std::endl;
            }
        }
        }).detach();
}
#endif

uint64_t Uart::load8(uint64_t addr) {
    auto& [mutex, cvar, uartArray] = *uart;
    std::unique_lock<std::mutex> lock(mutex);

    switch (addr) {
    case UART_RHR: {
        cvar.notify_one();
        uartArray[(UART_LSR - UART_BASE)] &= ~UART_LSR_RX;
        return uartArray[(UART_RHR - UART_BASE)];
    }
    default:
        return uartArray[(addr - UART_BASE)];   
    }
}

void Uart::store8(uint64_t addr, uint64_t value) {
    auto& [mutex, _, uartArray] = *uart;
    std::unique_lock<std::mutex> lock(mutex);

    switch (addr) {
    case UART_THR:
        std::cout.put(static_cast<char>(value & 0xFF));
        std::cout.flush();
        break;
    default:
        uartArray[(addr - UART_BASE)] = static_cast<uint8_t>(value & 0xFF);
    }
}

std::variant<uint64_t, Exception> Uart::store(uint64_t addr, uint64_t size, uint64_t value) {
    if (size == 8) {
        store8(addr, value);
        return (uint64_t)0;
    } else {
        return Exception::StoreAMOAccessFault;
    }
}

std::variant<uint64_t, Exception> Uart::load(uint64_t addr, uint64_t size) {
    if (size == 8) {
        return load8(addr);
    } else {
        return Exception::LoadAccessFault;
    }
}