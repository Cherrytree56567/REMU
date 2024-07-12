#define MACHINE_NAME Machine
#include "Machine.h"

MACHINE_NAME::MACHINE_NAME(int RamSize) {
	bus = std::make_shared<Bus>(RamSize);
	cpu = std::make_shared<CPU>(bus);
	cpu->adcp(std::make_any<std::shared_ptr<CPU>>(cpu));
}

bool MACHINE_NAME::Loop() {
	return cpu->Loop();
}