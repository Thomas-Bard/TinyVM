#ifndef IOMMU_HPP
#define IOMMU_HPP

#include <cstdint>
#include <array>
#include <functional>
#include <memory>
#include "cpu_commons.hpp"
#include <unordered_map>

#define IOMMU_TABLE_HEADER_SIZE 2       // Table size (uint16_t) + Table base address (uint16_t)
#define IOMMU_TABLE_ENTRY_SIZE  3       // Device port (uint16_t) + Device mapped address (uint16_t) + device segment size (uint16_t)

#define IOMMU_HEADER_TABLE_ADDR_OFFSET 0
#define IOMMU_HEADER_TABLE_SZ_OFFSET 1

#define IOMMU_ENTRY_DEVICE_PORT_OFFSET 0
#define IOMMU_ENTRY_DEVICE_ADDR_OFFSET 1
#define IOMMU_ENTRY_DEVICE_SIZE_OFFSET 2

/** /!\ TABLE SIZE IS IN ENTRIES NOT WORDS /!\ **/
/** Table size in words is TABLE_SIZE * IOMMU_TABLE_ENTRY_SIZE **/

namespace cpu::IOMMU
{
    typedef std::function<void(uint16_t, uint16_t, uint16_t)> iommu_write_callback_t;
    typedef std::function<uint16_t(uint16_t, uint16_t)> iommu_read_callback_t;

    typedef struct s_table_entry
    {
        uint16_t base_addr;
        uint16_t segment_size;
        uint16_t port;
    } table_entry_t;

    class IOMMU
    {
        public:
            IOMMU(MemoryReadCallback read_callback,
                MemoryWriteCallback write_callback);

            void device_write(
                uint16_t port,
                uint16_t virtual_addr,
                uint16_t word) const noexcept;
            uint16_t device_read(
                uint16_t port,
                uint16_t virtual_addr) const noexcept;

            void p_cpu_load_table_addr(uint16_t header_addr);
        private:
            std::unordered_map<uint16_t, table_entry_t> m_mmio_table_lookup;

            MemoryReadCallback m_memory_read;
            MemoryWriteCallback m_memory_write;
    };
}

#endif
