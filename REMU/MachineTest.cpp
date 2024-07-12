#define MACHINE_NAME MachineTest
#include "MachineTest.h"

MACHINE_NAME::MACHINE_NAME(int RamSize) : Machine(RamSize) {
	bus = std::make_shared<Bus>(RamSize);
	cpu = std::make_shared<CPUTest>(bus);
}

bool MACHINE_NAME::Loop() {
	return cpu->Loop();
}

void MACHINE_NAME::read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Unable to open file " << filename << std::endl;
        return;
    }

    std::streamsize fileLen = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(fileLen);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileLen)) {
        std::cerr << "Error reading file " << filename << std::endl;
        return;
    }

    // Print file contents in hex
    for (std::streamsize i = 0; i < fileLen; i += 2) {
        if (i % 16 == 0) std::cout << "\n" << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
        if (i + 1 < fileLen)
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i + 1]) << " ";
    }
    std::cout << std::dec << "\n";

    std::memcpy(bus->GetRam()->ReturnMemory()->data(), buffer.data(), fileLen * sizeof(uint8_t));
}