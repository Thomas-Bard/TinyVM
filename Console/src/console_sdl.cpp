#include "console_sdl.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

namespace console
{
    constexpr uint16_t DEFAULT_CELL_WIDTH = 8;
    constexpr uint16_t DEFAULT_CELL_HEIGHT = 16;

    constexpr uint16_t DEFAULT_WINDOW_WIDTH = 800;
    constexpr uint16_t DEFAULT_WINDOW_HEIGHT = 1000;

    ConsoleRendererSDL::ConsoleRendererSDL(uint16_t w, uint16_t h, std::string font_path)
        : ConsoleRenderer(w, h), _font_path(font_path), _should_quit(false)
    {
        TTF_Init();
        SDL_Init(SDL_INIT_VIDEO);
        _window = SDL_CreateWindow("Console Output", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0);
        _renderer = SDL_CreateRenderer(_window, NULL);
        _font_size = 70;
        _console_rect = SDL_Rect{0, 0, w * DEFAULT_CELL_WIDTH, h * DEFAULT_CELL_HEIGHT};
        _font_cache.reserve(256);
        auto font = TTF_OpenFont(font_path.c_str(), _font_size);
        for (uint16_t i = 0; i < 256; ++i) {
            _font_cache.push_back(TTF_RenderGlyph_Blended(font, (uint8_t)i, SDL_Color{255, 255, 255, 255}));
        }
        TTF_CloseFont(font);
    }

    ConsoleRendererSDL::~ConsoleRendererSDL()
    {
        std::cout << "~ConsoleRendererSDL()" << std::endl;
        SDL_DestroyWindow(_window);
        for (auto& surface : _font_cache) {
            SDL_DestroySurface(surface);
        }
        TTF_Quit();
        SDL_Quit();
    }

    void ConsoleRendererSDL::render_frame(const std::shared_ptr<char[]> data)
    {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
        SDL_RenderClear(_renderer);
        for (uint16_t y = 0; y < height; ++y) {
            for (uint16_t x = 0; x < width; ++x) {
                uint8_t char_code = static_cast<uint8_t>(data[y * width + x]);
                SDL_Surface* surface = _font_cache[char_code];
                SDL_FRect dest_rect = {(float)x * DEFAULT_CELL_WIDTH, (float)y * DEFAULT_CELL_HEIGHT, DEFAULT_CELL_WIDTH, DEFAULT_CELL_HEIGHT};
                SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
                SDL_RenderTexture(_renderer, texture, NULL, &dest_rect);
                SDL_DestroyTexture(texture);
            }
        }
        SDL_RenderPresent(_renderer);
    }

    void ConsoleRendererSDL::update()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                _should_quit = true;
            }
        }
    }

    bool ConsoleRendererSDL::should_quit() const
    {
        return _should_quit;
    }

    void ConsoleRendererSDL::clear()
    {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);
    }
}
