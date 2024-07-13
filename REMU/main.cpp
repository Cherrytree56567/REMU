/*
* MIT LICENCE
* 
* Copyright (c) 2024 Ronit D'silva

* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
* REFERENCES:
* https://github.com/fmash16/riscv_emulator
* https://book.rvemu.app/instruction-set/01-rv64i.html
* https://fmash16.github.io/content/posts/riscv-emulator-in-c.html
* https://github.com/d0iasm/rvemu-for-book
* https://five-embeddev.com/riscv-user-isa-manual/Priv-v1.12/instr-table.html
* https://msyksphinz-self.github.io/riscv-isadoc/html/index.html
* https://github.com/mit-pdos/xv6-riscv/blob/riscv/kernel/virtio_disk.c
*/
#include <iostream>
#include "Machine.h"

int main(int argc, char *argv[]) {
    bool debug = false; // argv[2]
    std::shared_ptr<REMUMAC::Machine> mac = std::make_shared<REMU::Machine>("helloworld.bin", "helloworld.bin", 1500);
    mac->ReturnCPU()->EnableDebugging(debug);

    while (mac->Loop()) {
        mac->ReturnCPU()->DumpRegisters();
    }
    return 0;
}