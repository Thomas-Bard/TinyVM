#ifndef IO_HPP
#define IO_HPP

#include <cstdint>
#include <memory>
#include <vector>

namespace TinyVM {
    namespace IO {
        class IODevice {
        public:
            IODevice(const uint16_t address);
            virtual ~IODevice() = default;

            const uint16_t address;

            virtual uint16_t read(const uint16_t port) = 0;
            virtual void write(const uint16_t port, const uint16_t data) = 0;
        };

        class IOManager {
        public:
            IOManager();
            void addDevice(std::unique_ptr<IODevice> device);
            uint16_t read(const uint16_t address, const uint16_t port);
            void write(const uint16_t address, const uint16_t port, const uint16_t data);
            IODevice* getDevice(const uint16_t address);
        private:
            std::vector<std::unique_ptr<IODevice>> m_devices;
        };
    }
}

#endif
