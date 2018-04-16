#pragma once

#include "glad/glad.h"

namespace Engine::GL {
    struct Texture {
        GLuint m_id;

        explicit Texture(const char* path);

        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator= (const Texture&) = delete;

        int m_width, m_height;

        float m_invWidth, m_invHeight;

        void Bind();
    };
}