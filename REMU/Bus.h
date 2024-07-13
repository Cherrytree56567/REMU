#pragma once
#include <variant>
#include <sstream>
#include "RAM.h"
#include "Plic.h"
#include "Clint.h"
#include "Uart.h"
#include "Virtio.h"

#define MEMORY_BASE 0x80000000

#define PLIC_BASE 0xc000000
#define PLIC_SIZE 0x4000000

#define CLINT_BASE 0x2000000
#define CLINT_SIZE 0x10000

#define UART_BASE 0x10000000
#define UART_SIZE 0x100

#define VIRTIO_BASE 0x10001000
#define VIRTIO_SIZE 0x1000

class Bus {
public:
	Bus(size_t size, std::vector<uint8_t> disk_image) : ram(std::make_shared<RAM>(size)), plic(std::make_shared<Plic>()), clint(std::make_shared<Clint>()), uart(std::make_shared<Uart>()), virtio(std::make_shared<Virtio>(disk_image)), RamSize(size) {}

	std::shared_ptr<RAM> GetRam() { return ram; }
	std::shared_ptr<Plic> GetPlic() { return plic; }
	std::shared_ptr<Clint> GetClint() { return clint; }
	std::shared_ptr<Uart> GetUart() { return uart; }
	std::shared_ptr<Virtio> GetVirtio() { return virtio; }

	std::variant<uint64_t, Exception> Load(uint64_t addr, uint64_t size);
    std::variant<uint64_t, Exception> Store(uint64_t addr, uint64_t size, uint64_t value);

private:
	std::shared_ptr<RAM> ram;
	std::shared_ptr<Plic> plic;
	std::shared_ptr<Clint> clint;
	std::shared_ptr<Uart> uart;
	std::shared_ptr<Virtio> virtio;

	size_t RamSize;
};

