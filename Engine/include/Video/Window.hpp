#include <string>

#include <SDL2/SDL.h>

namespace Engine::GL {
    class Window {
        SDL_Window* window;

        SDL_GLContext ctx;
    public:
        explicit Window(
                const char* title,
                const unsigned width = 1920,
                const unsigned height = 1080,
                const bool fullscreen = true,
                const bool msaa = false,
                const unsigned msaa_samples = 0
        );

        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void Present();
    };
}
