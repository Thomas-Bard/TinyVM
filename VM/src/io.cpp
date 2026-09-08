#include "io.hpp"

namespace TinyVM::IO {
    IODevice::IODevice(const uint16_t addr)
        : address(addr)
    {}

    IOManager::IOManager()
        : m_devices()
    {}

    void IOManager::addDevice(std::unique_ptr<IODevice> device) {
        m_devices.push_back(std::move(device));
    }

    uint16_t IOManager::read(const uint16_t address, const uint16_t port) {
        for (const auto& device : m_devices) {
            if (device->address == address) {
                return device->read(port);
            }
        }
        return 0;
    }

    void IOManager::write(const uint16_t address, const uint16_t port, const uint16_t data) {
        for (const auto& device : m_devices) {
            if (device->address == address) {
                device->write(port, data);
                return;
            }
        }
    }
}
