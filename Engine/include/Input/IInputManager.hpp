#pragma once

#include <glm/glm.hpp>

namespace Engine::Input {
    enum Keys {
        W,
        A,
        S,
        D,
        Mouse1,
        F1,
        F2
    };

    class IInputManager {
    public:
        virtual ~IInputManager() = default;

        virtual void Update() = 0;

        virtual bool GetKeyState(Keys key) = 0;

        virtual void SetKeyState(Keys key, bool state) = 0;

        virtual glm::vec2 GetMousePos() = 0;

        virtual glm::vec2 GetMouseDelta() = 0;

        virtual bool IsQuitRequested() = 0;
    };
}