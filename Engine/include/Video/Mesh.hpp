#pragma once

#include <vector>

#include <glm/glm.hpp>
#include "glad/glad.h"

namespace Engine::GL {
    class Mesh {
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_ebo;

        size_t m_drawCount;
    public:
        Mesh(
                  const std::vector<glm::vec3>& pos
                , const std::vector<glm::vec2>& uv
                , const std::vector<glm::vec3>& normal
                , const std::vector<uint32_t>& index
        );

        Mesh(const Mesh&) = delete;
        Mesh operator =(const Mesh&) = delete;
    };
}