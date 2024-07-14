#include "Machine.h"

namespace REMUMAC {
    Machine::Machine(std::string filename, std::string diskname, int RamSize) {
        std::ifstream file(diskname, std::ios::binary | std::ios::ate);
        if (!file) {
            std::cerr << "Unable to open file " << diskname << std::endl;
            return;
        }

        std::streamsize fileLen = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(fileLen);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), fileLen)) {
            std::cerr << "Error reading file " << diskname << std::endl;
            return;
        }
        bus = std::make_shared<Bus>(RamSize, buffer);
        cpu = std::make_shared<CPU>(bus);
        cpu->adcp(std::make_any<std::shared_ptr<CPU>>(cpu));
        read_file(filename);
    }

    bool Machine::Loop() {
        return cpu->Loop();
    }

    void Machine::read_file(const std::string& filename) {
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

        std::memcpy(bus->GetRam()->ReturnMemory()->data(), buffer.data(), fileLen * sizeof(uint8_t));
    }
}