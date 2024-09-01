#pragma once
#define VIRTIO_IRQ 1
#define VRING_DESC_SIZE 16
#define DESC_NUM 8
#define VIRTIO_BASE 0x10001000
#define VIRTIO_SIZE 0x1000
#define VIRTIO_MAGIC VIRTIO_BASE + 0x000
#define VIRTIO_VERSION VIRTIO_BASE + 0x004
#define VIRTIO_DEVICE_ID VIRTIO_BASE + 0x008
#define VIRTIO_VENDOR_ID VIRTIO_BASE + 0x00c
#define VIRTIO_DEVICE_FEATURES VIRTIO_BASE + 0x010
#define VIRTIO_DRIVER_FEATURES VIRTIO_BASE + 0x020
#define VIRTIO_GUEST_PAGE_SIZE VIRTIO_BASE + 0x028
#define VIRTIO_QUEUE_SEL VIRTIO_BASE + 0x030
#define VIRTIO_QUEUE_NUM_MAX VIRTIO_BASE + 0x034
#define VIRTIO_QUEUE_NUM VIRTIO_BASE + 0x038
#define VIRTIO_QUEUE_PFN VIRTIO_BASE + 0x040
#define VIRTIO_QUEUE_NOTIFY VIRTIO_BASE + 0x050
#define VIRTIO_STATUS VIRTIO_BASE + 0x070
#include <cstdint>
#include <memory>
#include <variant>
#include <vector>
#include "Device.h"

class Virtio : public Device {
public:
    Virtio(std::vector<uint8_t> disk_image) : id(0),
        driver_features(0),
        page_size(0),
        queue_sel(0),
        queue_num(0),
        queue_pfn(0),
        queue_notify(9999), // What is the initial value?
        status(0),
        disk(disk_image) {}

    virtual std::variant<uint64_t, Exception> load(uint64_t addr, uint64_t size) override;
    virtual std::variant<uint64_t, Exception> store(uint64_t addr, uint64_t size, uint64_t value) override;

    bool is_interrupting();
    
    uint64_t load32(uint64_t addr);
    
    void store32(uint64_t addr, uint64_t value);

    uint64_t get_new_id();

    uint64_t desc_addr();

    uint64_t read_disk(uint64_t addr);

    void write_disk(uint64_t addr, uint64_t value);

    void disk_access(std::any an);

    uint64_t id;
    uint32_t driver_features;
    uint32_t page_size;
    uint32_t queue_sel;
    uint32_t queue_num;
    uint32_t queue_pfn;
    uint32_t queue_notify;
    uint32_t status;
    std::vector<uint8_t> disk;
};