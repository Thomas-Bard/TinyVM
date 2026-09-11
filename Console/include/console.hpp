#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <cstdint>
#include <memory>

namespace console {
    class ConsoleRenderer {
    public:
        ConsoleRenderer(uint16_t w, uint16_t h);
        virtual void render_frame(std::shared_ptr<char[]>) = 0;
        virtual void clear() = 0;
        virtual ~ConsoleRenderer() = default;

        const uint16_t width;
        const uint16_t height;
    };
}

#endif
