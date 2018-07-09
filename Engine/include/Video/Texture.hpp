#pragma once

#include "glad/glad.h"

namespace Engine::GL {
    struct Texture {
        GLuint m_id;

        explicit Texture(const char* path);

        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator= (const Texture&) = delete;

        Texture(Texture&& other)
        {
            // GL is kind of a piece of shit, therefore we need to move the ID out so it doesn't double free
            m_id = other.m_id;
            other.m_id = 0;

            m_width = other.m_width;
            m_height = other.m_height;
        }

        int m_width, m_height;

        void Bind(unsigned unit);

        void Unbind();

        static void BindNull(unsigned unit);
    };
}