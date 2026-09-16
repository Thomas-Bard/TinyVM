#include "mmio_device.hpp"
#include "iommu.hpp"

namespace cpu::IOMMU::devices
{
    MMIO_Device::MMIO_Device(uint16_t port,
        iommu_read_callback_t read_callback,
        iommu_write_callback_t write_callback
    )
    : m_port(port), m_iommu_read(read_callback), m_iommu_write(write_callback)
    {}

    MMIO_Device::MMIO_Device(void)
        : m_port(0), m_iommu_read(nullptr), m_iommu_write(nullptr)
    {}

    uint16_t MMIO_Device::m_read(uint16_t addr) const noexcept
    {
        if (!m_iommu_read)
            return 0;
        return m_iommu_read(m_port, addr);
    }

    void MMIO_Device::m_write(uint16_t addr, uint16_t data) const noexcept
    {
        if (!m_iommu_write)
            return;
        m_iommu_write(m_port, addr, data);
    }

    void MMIO_Device::set_port(uint16_t port) noexcept
    {
        m_port = port;
    }

    uint16_t MMIO_Device::get_port(void) const noexcept
    {
        return m_port;
    }

    void MMIO_Device::set_iommu_read_callback(iommu_read_callback_t callback) noexcept
    {
        m_iommu_read = callback;
    }

    void MMIO_Device::set_iommu_write_callback(iommu_write_callback_t callback) noexcept
    {
        m_iommu_write = callback;
    }
}
