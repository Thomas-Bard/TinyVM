#include <console_sdl.hpp>
#include <iterator>
#include <memory>


int main(int argc, char** argv)
{
    if (argc < 2)
        return 1;

    const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    console::ConsoleRendererSDL renderer(80, 60, argv[1]);

    std::shared_ptr<char[]> framebuffer = std::make_shared<char[]>(80 * 60);
    size_t i = 0;
    for (uint16_t y = 0; y < 60; ++y) {
        for (uint16_t x = 0; x < 80; ++x) {
            framebuffer[y * 80 + x] = alphabet[i % (sizeof(alphabet) - 1)];
            ++i;
        }
    }

    renderer.render_frame(framebuffer);
    while (!renderer.should_quit()) {
        renderer.update();
    }
}
