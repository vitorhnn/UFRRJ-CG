#pragma once

#include <string>

#include "Video/Mesh.hpp"

namespace Engine::Util::ObjLoader {
    std::vector<std::string> Tokenize(const std::string& str, const char separator);

    class OBJModel {
        struct Index {
            unsigned vertex;
            unsigned normal;
            unsigned uv;

            bool operator ==(const Index& rhs) const
            {
                return vertex == rhs.vertex &&
                       normal == rhs.vertex &&
                       uv == rhs.uv;
            }
        };

        std::vector<glm::vec3> m_vertices;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec2> m_uvs;
        std::vector<Index> m_indices;

        bool m_hasUVs = false;
        bool m_hasNormals = false;

        void ReadIndex(const std::string& line);

    public:
        explicit OBJModel(const std::string& fileData);

        Engine::GL::Mesh Upload();
    };
}
