#include "Virtio.h"
#include "CPU.h"

std::variant<uint64_t, Exception> Virtio::load(uint64_t addr, uint64_t size) {
    switch (size) {
    case 32: return load32(addr); break;
    default: return Exception::LoadAccessFault; break;
    }
}

std::variant<uint64_t, Exception> Virtio::store(uint64_t addr, uint64_t size, uint64_t value) {
    switch (size) {
    case 32: store32(addr, value); break;
    default: return Exception::StoreAMOAccessFault; break;
    }
    return (uint64_t)0;
}

bool Virtio::is_interrupting() {
    if (queue_notify != 9999) {
        queue_notify = 9999;
        return true;
    }
    return false;
}

uint64_t Virtio::load32(uint64_t addr) {
    switch (addr) {
    case VIRTIO_MAGIC: return 0x74726976; break;
    case VIRTIO_VERSION: return 0x1; break;
    case VIRTIO_DEVICE_ID: return 0x2; break;
    case VIRTIO_VENDOR_ID: return 0x554d4551; break;
    case VIRTIO_DEVICE_FEATURES: return 0; break; // What should it return?
    case VIRTIO_DRIVER_FEATURES: return (uint64_t)driver_features; break;
    case VIRTIO_QUEUE_NUM_MAX: return 8; break;
    case VIRTIO_QUEUE_PFN: return (uint64_t)queue_pfn; break;
    case VIRTIO_STATUS: return (uint64_t)status; break;
    default: return 0; break;
    }
}

void Virtio::store32(uint64_t addr, uint64_t value) {
    uint32_t val = (uint32_t)value;
    switch (addr) {
    case VIRTIO_DEVICE_FEATURES: driver_features = val; break;
    case VIRTIO_GUEST_PAGE_SIZE: page_size = val; break;
    case VIRTIO_QUEUE_SEL: queue_sel = val; break;
    case VIRTIO_QUEUE_NUM: queue_num = val; break;
    case VIRTIO_QUEUE_PFN: queue_pfn = val; break;
    case VIRTIO_QUEUE_NOTIFY: queue_notify = val; break;
    case VIRTIO_STATUS: status = val; break;
    default: break;
    }
}

uint64_t Virtio::get_new_id() {
    id += 1;
    return id;
}

uint64_t Virtio::desc_addr() {
    return ((uint64_t)queue_pfn) * ((uint64_t)page_size);
}

uint64_t Virtio::read_disk(uint64_t addr) {
    return (uint64_t)disk[addr];
}

void Virtio::write_disk(uint64_t addr, uint64_t value) {
    disk[addr] = (uint8_t)value;
}

void Virtio::disk_access(std::any an) {
    std::shared_ptr<CPU> cpu = std::any_cast<std::shared_ptr<CPU>>(an);
    uint64_t descaddr = desc_addr();
    uint64_t avail_addr = desc_addr() + 0x40;
    uint64_t used_addr = desc_addr() + 4096;

    uint64_t offset;
    auto res = cpu->bus->Load(avail_addr + 1, 16);
    if (std::holds_alternative<uint64_t>(res)) {
        offset = std::get<uint64_t>(res);
    } else {
        offset = 0;
        std::cout << "Failed to read offset\n";
    }

    uint64_t index;
    auto ress = cpu->bus->Load(avail_addr + (offset % DESC_NUM) + 2, 16);
    if (std::holds_alternative<uint64_t>(ress)) {
        index = std::get<uint64_t>(ress);
    } else {
        index = 0;
        std::cout << "Failed to read index\n";
    }

    uint64_t desc_addr0 = descaddr + VRING_DESC_SIZE * index;
    uint64_t addr0;
    auto resss = cpu->bus->Load(desc_addr0, 64);
    if (std::holds_alternative<uint64_t>(resss)) {
        addr0 = std::get<uint64_t>(resss);
    } else {
        addr0 = 0;
        std::cout << "Failed to read an address field in a descriptor\n";
    }

    uint64_t next0;
    auto ressss = cpu->bus->Load(desc_addr0 + 14, 16);
    if (std::holds_alternative<uint64_t>(ressss)) {
        next0 = std::get<uint64_t>(ressss);
    } else {
        next0 = 0;
        std::cout << "Failed to read a next field in a descriptor\n";
    }

    uint64_t desc_addr1 = descaddr + VRING_DESC_SIZE * next0;
    uint64_t addr1;
    auto resssss = cpu->bus->Load(desc_addr1, 64);
    if (std::holds_alternative<uint64_t>(resssss)) {
        addr1 = std::get<uint64_t>(resssss);
    } else {
        addr1 = 0;
        std::cout << "Failed to read an address field in a descriptor\n";
    }

    uint64_t len1;
    auto asx = cpu->bus->Load(desc_addr1 + 8, 32);
    if (std::holds_alternative<uint64_t>(asx)) {
        len1 = std::get<uint64_t>(asx);
    } else {
        len1 = 0;
        std::cout << "Failed to read a length field in a descriptor\n";
    }

    uint64_t flags1;
    auto abx = cpu->bus->Load(desc_addr1 + 12, 16);
    if (std::holds_alternative<uint64_t>(abx)) {
        flags1 = std::get<uint64_t>(abx);
    } else {
        flags1 = 0;
        std::cout << "Failed to read a flags field in a descriptor\n";
    }

    uint64_t blk_sector;
    auto aax = cpu->bus->Load(addr0 + 8, 64);
    if (std::holds_alternative<uint64_t>(aax)) {
        blk_sector = std::get<uint64_t>(aax);
    } else {
        blk_sector = 0;
        std::cout << "Failed to read a sector field in a virtio_blk_outhdr\n";
    }

    if ((flags1 & 2) == 0) {
        for (uint64_t i = 0; i < len1; ++i) {
            uint64_t data;
            auto aaax = cpu->bus->Load(addr1 + i, 8);
            if (std::holds_alternative<uint64_t>(aaax)) {
                data = std::get<uint64_t>(aaax);
            } else {
                data = 0;
                std::cout << "Failed to read from dram\n";
            }
            write_disk(blk_sector * 512 + i, data);
        }
    } else {
        for (uint64_t i = 0; i < len1; ++i) {
            uint64_t data = read_disk(blk_sector * 512 + i);
            auto aaax = cpu->bus->Store(addr1 + i, 8, data);
            if (!std::holds_alternative<uint64_t>(aaax)) {
                std::cout << "Failed to write to dram\n";
            }
        }
    }
    uint64_t new_id = get_new_id();
    auto vaax = cpu->bus->Store(used_addr + 2, 16, new_id % 8);
    if (!std::holds_alternative<uint64_t>(vaax)) {
        std::cout << "Failed to write to dram\n";
    }
}