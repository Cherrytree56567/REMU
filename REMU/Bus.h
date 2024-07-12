#pragma once
#include "RAM.h"

class Bus {
public:
	Bus(size_t size) : ram(std::make_shared<RAM>(size)) {}

	std::shared_ptr<RAM> GetRam() { return ram; }

private:
	std::shared_ptr<RAM> ram;
};

