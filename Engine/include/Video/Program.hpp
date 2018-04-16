#pragma once

#include <string>
#include <map>

#include <glm/glm.hpp>

#include "glad/glad.h"

namespace Engine::GL {
    // this is non exhaustive, notably absent are geometry and tesselation shaders
    enum class ShaderType {
        Vertex,
        Fragment
    };

    class Program final {
    private:
        GLuint m_glid;

        std::map<std::string, GLint> m_uniforms; // using ordered maps because r-b trees are generally better for memory and string search (I think)

        GLint GetUniformLocation(const std::string& uniform);
    public:
        Program();

        ~Program();

        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;

        void Use();

        void AttachShader(const char* path, ShaderType type);

        void Link();

        void SetUniform(const std::string& uniform, const int value);

        void SetUniform(const std::string& uniform, const float value);

        void SetUniform(const std::string& uniform, const glm::vec4& vec);

        void SetUniform(const std::string& uniform, const glm::vec3& vec);

        void SetUniform(const std::string& uniform, const glm::mat4& mat);
    };

}