#ifndef CONSOLE_SDL_HPP
#define CONSOLE_SDL_HPP

#include "console.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>
#include <vector>

namespace console
{
    class ConsoleRendererSDL : public ConsoleRenderer
    {
    public:
        ConsoleRendererSDL(uint16_t w, uint16_t h, std::string font_path);
        ~ConsoleRendererSDL();
        void render_frame(std::shared_ptr<char[]>) override;
        void clear() override;
        void update();
        bool should_quit() const;
    private:
        std::vector<SDL_Surface*> _font_cache;
        std::string _font_path;
        float _font_size; // Calculated from width and height
        SDL_Window* _window;
        SDL_Renderer* _renderer;
        SDL_Rect _console_rect;
        bool _should_quit;
    };
}

#endif
