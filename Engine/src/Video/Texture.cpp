#include <cstdio>
#include <stdexcept>

#include "stb_image.h"

#include "Video/Texture.hpp"


namespace Engine::GL {
    Texture::Texture(const char* path) :
        m_id(0),
        m_width(0),
        m_height(0)
    {
        // TODO: maybe use FS:: ?
        FILE* fp = fopen(path, "rb");
        int channels;
        uint8_t* bytes = stbi_load_from_file(fp, &m_width, &m_height, &channels, 0);

        GLenum imgFormat;

        switch (channels) {
            case 3:
                imgFormat = GL_RGB;
                break;
            case 4:
                imgFormat = GL_RGBA;
                break;
            default:
                throw std::runtime_error("tried loading something with less than 3 channels? (grayscale?)");
        }

        glGenTextures(1, &m_id);
        printf("generated texture with id %u, name %s\n", m_id, path);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, imgFormat, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(bytes);
        fclose(fp);
    }

    Texture::~Texture()
    {
        if (m_id == 0) {
            return;
        }

        printf("texture id %u is dying\n", m_id);
        glDeleteTextures(1, &m_id);
    }

    void Texture::Bind(unsigned unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    void Texture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::BindNull(unsigned unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}