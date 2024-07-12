#pragma once
#include "CPUTest.h"
#include "RAM.h"
#include "Machine.h"

class MachineTest : public Machine {
public:
	MachineTest(int RamSize);

	virtual bool Loop() override;
	virtual std::shared_ptr<CPU> ReturnCPU() override { return cpu; }
	virtual std::shared_ptr<Bus> ReturnBUS() override { return bus; }
	
	void read_file(const std::string& filename);
private:
	std::shared_ptr<CPUTest> cpu;
	std::shared_ptr<Bus> bus;
};

