#pragma once

#include "Video/Texture.hpp"

#include <vector>

#include <glm/glm.hpp>
#include "glad/glad.h"

namespace Engine::GL {
    class Mesh {
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_ebo;

        Texture* m_diffuse;
        Texture* m_specular;
        Texture* m_bumpmap;
        Texture* m_displacementMap;

        size_t m_drawCount;
    public:
        Mesh(
                  const std::vector<glm::vec3>& pos
                , const std::vector<glm::vec2>& uv
                , const std::vector<glm::vec3>& normal
                , const std::vector<glm::vec3>& tangents
                , const std::vector<uint32_t>& index
                , Texture* diffuse
                , Texture* specular
                , Texture* bumpmap
                , Texture* displacementMap
        );

        Mesh(const Mesh&) = delete;
        Mesh operator =(const Mesh&) = delete;

        Mesh(Mesh&&) = default;

        // This is a bad abstraction but I cba to build a better one
        void Draw();
    };
}