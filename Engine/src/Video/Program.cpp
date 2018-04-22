/// @file
/// @author Victor Hermann "vitorhnn" Chiletto

#include <glm/gtc/type_ptr.hpp>

#include "Util/FS.hpp"

#include "Video/Program.hpp"

namespace Engine::GL {
    Program::Program() : m_glid(glCreateProgram())
    {
    }

    Program::~Program()
    {
        glDeleteProgram(m_glid);
    }

    void Program::Use()
    {
        glUseProgram(m_glid);
    }

    GLint Program::GetUniformLocation(const std::string& uniform)
    {
        auto maybeLocation = m_uniforms.find(uniform);

        if (maybeLocation != m_uniforms.end()) {
            return maybeLocation->second;
        }

        GLint location = glGetUniformLocation(m_glid, uniform.c_str());

        m_uniforms[uniform] = location;

        return location;
    }

    void Program::SetUniform(const std::string &uniform, const int value)
    {
        glUniform1i(GetUniformLocation(uniform), value);
    }

    void Program::SetUniform(const std::string& uniform, const float value)
    {
        glUniform1f(GetUniformLocation(uniform), value);
    }

    void Program::SetUniform(const std::string& uniform, const glm::vec4& vec)
    {
        glUniform4fv(GetUniformLocation(uniform), 1, glm::value_ptr(vec));
    }

    void Program::SetUniform(const std::string& uniform, const glm::vec3& vec)
    {
        glUniform3fv(GetUniformLocation(uniform), 1, glm::value_ptr(vec));
    }

    void Program::SetUniform(const std::string& uniform, const glm::mat3& mat)
    {
        glUniformMatrix3fv(GetUniformLocation(uniform), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Program::SetUniform(const std::string& uniform, const glm::mat4& mat)
    {
        glUniformMatrix4fv(GetUniformLocation(uniform), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Program::AttachShader(const char* path, ShaderType type)
    {
        // magically read from somewhere (hint hint, implement FS::)
        auto vec = Util::FS::ReadAllBytes(path);
        vec.push_back('\0');
        const GLchar* source = vec.data();

        GLenum gltype;

        switch (type) {
            case ShaderType::Fragment:
                gltype = GL_FRAGMENT_SHADER;
                break;
            case ShaderType::Vertex:
                gltype = GL_VERTEX_SHADER;
                break;
        }

        GLuint id = glCreateShader(gltype);
        glShaderSource(id, 1, &source, nullptr);
        glCompileShader(id);

        glAttachShader(m_glid, id);
        glDeleteShader(id);
    }

    void Program::Link()
    {
        glLinkProgram(m_glid);
    }
}