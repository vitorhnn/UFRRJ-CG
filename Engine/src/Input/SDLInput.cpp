#include "Input/SDLInput.hpp"

namespace Engine::Input {
    SDLInput::SDLInput() :
            KeyState(),
            m_shouldQuit(false),
            m_mousePos(0.0f, 0.0f),
            m_delta(0.0f, 0.0f)
    {
        for (auto& key: KeyState) {
            key = false;
        }
    }

    void SDLInput::Update()
    {
        SDL_Event ev;

        m_delta = {0, 0};

        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    m_shouldQuit = true;
                    break;
                case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym) {
                        case SDLK_w:
                            KeyState[Keys::W] = true;
                            break;
                        case SDLK_a:
                            KeyState[Keys::A] = true;
                            break;
                        case SDLK_s:
                            KeyState[Keys::S] = true;
                            break;
                        case SDLK_d:
                            KeyState[Keys::D] = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (ev.key.keysym.sym) {
                        case SDLK_w:
                            KeyState[Keys::W] = false;
                            break;
                        case SDLK_a:
                            KeyState[Keys::A] = false;
                            break;
                        case SDLK_s:
                            KeyState[Keys::S] = false;
                            break;
                        case SDLK_d:
                            KeyState[Keys::D] = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEMOTION: {
                    m_mousePos = {
                            static_cast<float>(ev.motion.x),
                            static_cast<float>(ev.motion.y)
                    };

                    glm::vec2 delta = {
                            static_cast<float>(ev.motion.xrel),
                            static_cast<float>(-1 * ev.motion.yrel)
                    };

                    m_delta += delta;

                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                    switch (ev.button.button) {
                        case SDL_BUTTON_LEFT:
                            KeyState[Keys::Mouse1] = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    switch (ev.button.button) {
                        case SDL_BUTTON_LEFT:
                            KeyState[Keys::Mouse1] = false;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    bool SDLInput::GetKeyState(Keys key)
    {
        return KeyState[key];
    }

    void SDLInput::SetKeyState(Keys key, bool state)
    {
        KeyState[key] = state;
    }

    glm::vec2 SDLInput::GetMousePos()
    {
        return m_mousePos;
    }

    glm::vec2 SDLInput::GetMouseDelta()
    {
        return m_delta;
    }

    bool SDLInput::IsQuitRequested()
    {
        return m_shouldQuit;
    }

}

