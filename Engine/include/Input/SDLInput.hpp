#pragma once

#include <array>

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "IInputManager.hpp"

namespace Engine::Input {
    class SDLInput final : public IInputManager {
        std::array<bool, sizeof(Keys) + 1> KeyState;

        bool m_shouldQuit = false;

        glm::vec2 m_mousePos;

        glm::vec2 m_delta;
    public:
        SDLInput();

        ~SDLInput() override = default;

        void Update() override;

        bool GetKeyState(Keys key) override;

        void SetKeyState(Keys key, bool state) override;

        glm::vec2 GetMousePos() override;

        glm::vec2 GetMouseDelta() override;

        bool IsQuitRequested() override;
    };
}