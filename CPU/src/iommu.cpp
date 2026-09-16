#include "iommu.hpp"
#include "cpu_commons.hpp"

#include <iostream>


namespace cpu::IOMMU
{
    IOMMU::IOMMU(MemoryReadCallback read_callback, MemoryWriteCallback write_callback)
        : m_memory_read(read_callback), m_memory_write(write_callback)
    {}

    uint16_t IOMMU::device_read(uint16_t port, uint16_t virtual_addr) const noexcept
    {
        if (!m_mmio_table_lookup.contains(port))
            return 0;
        const table_entry_t& entry = m_mmio_table_lookup.at(port);
        if (virtual_addr >= entry.segment_size)
            return 0;
        uint16_t word = m_memory_read(entry.base_addr + virtual_addr);
        return word;
    }

    void IOMMU::device_write(uint16_t port, uint16_t virtual_addr, uint16_t word) const noexcept
    {
        if (!m_mmio_table_lookup.contains(port))
            return;
        const table_entry_t& entry = m_mmio_table_lookup.at(port);
        if (virtual_addr >= entry.segment_size)
            return;
        m_memory_write(virtual_addr + entry.base_addr, word);
    }

    void IOMMU::p_cpu_load_table_addr(uint16_t header_addr)
    {
        m_mmio_table_lookup.clear();

        uint16_t table_addr = m_memory_read(header_addr + IOMMU_HEADER_TABLE_ADDR_OFFSET);
        uint16_t table_size = m_memory_read(header_addr + IOMMU_HEADER_TABLE_SZ_OFFSET);

        // Let's read the iommu table in memory and populate private table
        for (uint16_t entry = 0; entry < table_size; entry++)
        {
            table_entry_t table_entry;
            table_entry.base_addr = m_memory_read(table_addr + entry * IOMMU_TABLE_ENTRY_SIZE + IOMMU_ENTRY_DEVICE_ADDR_OFFSET);
            table_entry.port = m_memory_read(table_addr + entry * IOMMU_TABLE_ENTRY_SIZE + IOMMU_ENTRY_DEVICE_PORT_OFFSET);
            table_entry.segment_size = m_memory_read(table_addr + entry * IOMMU_TABLE_ENTRY_SIZE + IOMMU_ENTRY_DEVICE_SIZE_OFFSET);

            if ((uint32_t)table_entry.base_addr + table_entry.segment_size > 0xFFFF)
            {
                std::cerr << "[WARNING] | IOMMU | CPU tried to map an invalid memory region to device " << table_entry.port << " device will not be registered !" << std::endl;
                continue;
            }

            m_mmio_table_lookup[table_entry.port] = table_entry;
        }
    }
}
