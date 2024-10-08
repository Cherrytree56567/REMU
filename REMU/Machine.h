#pragma once
#include "CPU.h"
#include "RAM.h"

namespace REMUMAC {
	class Machine {
	public:
		Machine(std::string filename, std::string diskname, int RamSize);

		bool Loop();
		std::shared_ptr<CPU> ReturnCPU() { return cpu; }
		std::shared_ptr<Bus> ReturnBUS() { return bus; }

		void read_file(const std::string& filename);
	private:
		std::shared_ptr<CPU> cpu;
		std::shared_ptr<Bus> bus;
	};
}