#include "conout.hpp"
#include <cstdint>

namespace TinyVM::IO {
    ConOut::ConOut(const uint16_t addr, uint16_t width, uint16_t height, std::unique_ptr<console::ConsoleRenderer> renderer)
        : IODevice(addr), _width(width), _height(height), _renderer(std::move(renderer))
    {
        _buffer = std::make_shared<char[]>(width * height);
        clear();
    }

    void ConOut::write(const uint16_t port, const uint16_t data) {
        switch (port) {
            case CONOUT_PORT_CURSOR:
                setCursor(data & 0xFF, data >> 8);
                break;
            case CONOUT_PORT_CHAR:
                putChar(static_cast<char>(data));
                break;
            case CONOUT_PORT_DIM:
                // Dimensions are not modifiable at runtime
                break;
            default:
                break;
        }
    }

    uint16_t ConOut::read(const uint16_t port) {
        uint16_t data;
        switch (port) {
            case CONOUT_PORT_CURSOR:
                data = static_cast<uint16_t>((_cursorY << 8) | _cursorX);
                break;
            case CONOUT_PORT_CHAR:
                data = static_cast<uint16_t>(_buffer[_cursorX + _cursorY * _width]);
                break;
            case CONOUT_PORT_DIM:
                data = static_cast<uint16_t>((_height << 8) | _width);
                break;
            default:
                data = 0;
                break;
        }
        return data;
    }

    void ConOut::setCursor(uint16_t x, uint16_t y) {
        _cursorX = x;
        _cursorY = y;
    }
    void ConOut::putChar(char c) {
        if (_cursorX >= _width || _cursorY >= _height)
            return;
        _buffer[_cursorX + _cursorY * _width] = c;
    }
    void ConOut::clear() {
        std::fill(_buffer.get(), _buffer.get() + _width * _height, 0);
    }
    void ConOut::display() {
        _renderer->render_frame(_buffer);
    }

    console::ConsoleRenderer* ConOut::getRenderer(void) {
        return _renderer.get();
    }
}
