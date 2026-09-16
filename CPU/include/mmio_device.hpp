#ifndef MMIO_DEVICE_HPP
#define MMIO_DEVICE_HPP

#include "iommu.hpp"

namespace cpu::IOMMU::devices
{
    class MMIO_Device
    {
        public:
            MMIO_Device(
                uint16_t port,
                iommu_read_callback_t iommu_read,
                iommu_write_callback_t iommu_write
            );

            MMIO_Device(void);

            virtual void update(void) = 0;

            void set_port(uint16_t port) noexcept;
            uint16_t get_port(void) const noexcept;

            void set_iommu_read_callback(iommu_read_callback_t callback) noexcept;
            void set_iommu_write_callback(iommu_write_callback_t callback) noexcept;

        private:
            uint16_t m_read(uint16_t addr) const noexcept;
            void m_write(uint16_t addr, uint16_t word) const noexcept;

            uint16_t m_port;
            iommu_read_callback_t m_iommu_read;
            iommu_write_callback_t m_iommu_write;
    };
}

#endif
