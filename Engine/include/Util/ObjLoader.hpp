#pragma once

#include <string>

#include "Video/Mesh.hpp"
#include "Video/Model.hpp"

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

        struct OBJMesh {
            OBJMesh(const std::vector<std::string>& lines
                    , std::vector<glm::vec3>& vertices
                    , std::vector<glm::vec3>& normals
                    , std::vector<glm::vec2>& uvs
                    , std::vector<glm::vec3>& tangents
            );


            std::string m_material;

            std::vector<glm::vec3>& m_vertices;
            std::vector<glm::vec3>& m_normals;
            std::vector<glm::vec2>& m_uvs;
            std::vector<glm::vec3>& m_tangents;
            std::vector<Index> m_indices;

            bool m_hasUVs = false;
            bool m_hasNormals = false;

            void ReadIndex(const std::string& line);

            void GenerateNormals();

            void GenerateTangents();

            Engine::GL::Mesh Upload(GL::Texture* diffuse, GL::Texture* specular, GL::Texture* bump);
        };

        struct Material {
            Material(std::string name) : name(name) {}

            float specularPower;

            std::string name;
            std::string ambientMap;
            std::string diffuseMap;
            std::string specularMap;
            std::string bumpMap;
        };

        void ParseMtl(const std::string& path);

        std::vector<glm::vec3> m_vertices;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec2> m_uvs;
        std::vector<glm::vec3> m_tangents;

        std::vector<Material> m_materials;

        std::vector<OBJMesh> m_meshes;
    public:
        explicit OBJModel(const std::string& fileData);

        Engine::GL::Model Upload();
    };
}
