#define MACHINE_NAME Machine
#include "Machine.h"

MACHINE_NAME::MACHINE_NAME(int RamSize) {
	ram = std::make_shared<RAM>(RamSize);
	cpu = std::make_shared<CPU>(ram);
}

bool MACHINE_NAME::Loop() {
	return cpu->Loop();
}