#ifndef IO_HPP
#define IO_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <functional>

namespace TinyVM {
    namespace IO {
        typedef std::function<void(uint16_t)> ExternalEventHandler;

        class IODevice {
        public:
            IODevice(const uint16_t address);
            virtual ~IODevice() = default;

            const uint16_t address;

            virtual uint16_t read(const uint16_t port) = 0;
            virtual void write(const uint16_t port, const uint16_t data) = 0;
            virtual void update(void) = 0;

            bool external_event_handling;
        };

        class IOManager {
        public:
            IOManager(ExternalEventHandler hndl);
            void addDevice(std::unique_ptr<IODevice> device);
            uint16_t read(const uint16_t address, const uint16_t port);
            void write(const uint8_t port, const uint16_t address, const uint16_t data);
            IODevice* getDevice(const uint16_t address);
            void update_devices(void);
        private:
            std::vector<std::unique_ptr<IODevice>> m_devices;
            ExternalEventHandler m_external_handler;
        };
    }
}

#endif
