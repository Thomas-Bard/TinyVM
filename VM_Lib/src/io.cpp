#include "io.hpp"

namespace TinyVM::IO {
    IODevice::IODevice(const uint16_t addr)
        : address(addr)
    {}

    IOManager::IOManager(ExternalEventHandler hndl)
        : m_devices(), m_external_handler(hndl)
    {}

    void IOManager::addDevice(std::unique_ptr<IODevice> device)
    {
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

    void IOManager::write(const uint8_t port, const uint16_t address, const uint16_t data) {
        for (const auto& device : m_devices) {
            if (device->address == address) {
                device->write(port, data);
                return;
            }
        }
    }

    IODevice* IOManager::getDevice(const uint16_t address) {
        for (const auto& device : m_devices) {
            if (device->address == address) {
                return device.get();
            }
        }
        return nullptr;
    }

    void IOManager::update_devices(void)
    {
        for (auto& device : m_devices)
        {
            device->update();
            if (device->external_event_handling)
                m_external_handler(device->address);
        }
    }
}
