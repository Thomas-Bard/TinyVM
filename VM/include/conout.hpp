#ifndef CONOUT_HPP
#define CONOUT_HPP

#include <algorithm>
#include <cstdint>
#include "io.hpp"
#include "console.hpp"

/* ConOut: Virtual Console output device */
/* Address (VM-defined)
 * Ports:
 *  - 0x00 R/W - Uint16 - XY Coordinates of the cursor (LSB = x, MSB = y)
 *  - 0x01 R/W - Uint16 - Character to display (ASCII, MSB is ignored)
 *  - 0x02 R - Uint16 - Console dimensions (width, height) (LSB = width, MSB = height)
 */

namespace TinyVM {
    namespace IO {

        constexpr uint16_t CONOUT_PORT_CURSOR = 0x00;
        constexpr uint16_t CONOUT_PORT_CHAR = 0x01;
        constexpr uint16_t CONOUT_PORT_DIM = 0x02;

        constexpr uint8_t font_width = 8;
        constexpr uint8_t font_height = 16;

        class ConOut : public IODevice {
        public:
            ConOut(uint16_t address, uint16_t width, uint16_t height, std::unique_ptr<console::ConsoleRenderer> renderer);
            void write(uint16_t port, uint16_t value) override;
            uint16_t read(uint16_t port) override;

            void setCursor(uint16_t x, uint16_t y);
            void putChar(char c);
            void clear();
            void display();

            console::ConsoleRenderer* getRenderer(void);
        private:
            uint16_t _cursorX;
            uint16_t _cursorY;
            uint16_t _width;
            uint16_t _height;
            std::shared_ptr<char[]> _buffer;
            std::unique_ptr<console::ConsoleRenderer> _renderer;
        };
    }
}

#endif
