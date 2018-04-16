#include <stdexcept>

#include "glad/glad.h"

#include "Video/Window.hpp"

namespace Engine::GL {
    Window::Window(
            const char* title,
            const unsigned width,
            const unsigned height,
            const bool fullscreen,
            const bool msaa,
            const unsigned msaa_samples
    )
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        if (msaa) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa_samples);
        }

        uint32_t flags = SDL_WINDOW_OPENGL;

        if (fullscreen) {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }

        window = SDL_CreateWindow(title,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  width,
                                  height,
                                  flags
        );

        if (window == nullptr) {
            throw std::runtime_error("failed to create window");
        }

        ctx = SDL_GL_CreateContext(window);

        if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            throw std::runtime_error("failed to load gl");
        }

        SDL_GL_SetSwapInterval(-1);
    }

    Window::~Window()
    {
        SDL_GL_DeleteContext(ctx);
        SDL_DestroyWindow(window);
    }

    void Window::Present()
    {
        SDL_GL_SwapWindow(window);
    }
}
