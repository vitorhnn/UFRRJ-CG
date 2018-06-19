/// @file
/// Contains all the .obj file loader functions
/// @author Victor Hermann "vitorhnn" Chiletto

#include "Util/ObjLoader.hpp"
#include "Util/FS.hpp"

#include "Video/Texture.hpp"

#include <algorithm>
#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>

/// Container namespace for the .obj loader
namespace Engine::Util::ObjLoader {

    // TODO: this is a horrible hack. build a better resource manager
    static std::unordered_map<std::string, GL::Texture> __textures;

    /// Separates a string into multiple strings, using the given separator
    /// @param str The string to tokenize
    /// @param separator The character to separate the string with
    /// @todo This *might* be useful elsewhere, maybe move this to some other file?
    /// @returns A vector of tokens
    std::vector<std::string> Tokenize(const std::string& str, const char separator)
    {
        std::vector<std::string> tokens;

        std::string::size_type pos, lastPos = 0, len = str.length();

        while (lastPos < len + 1) {
            pos = str.find_first_of(separator, lastPos);

            if (pos == std::string::npos) {
                pos = len;
            }

            if (pos != lastPos) {
                tokens.emplace_back(std::string{str.data() + lastPos, pos - lastPos});
            }

            lastPos = pos + 1;
        }

        return tokens;
    }

    /// Reads .obj formatted 3 element vertex data
    /// @param line A line containing a 3 element vertex
    /// @returns A 3 element GLM vector
    /// @throws std::out_of_range if there are less than 3 elements in the line (from std::vector::at)
    /// @throws std::invalid_argument if an element is not a float (from std::string:stof)
    static glm::vec3 ReadVertex3(const std::string &line)
    {
        auto components = Tokenize(line, ' ');

        auto x = std::stof(components.at(1)),
            y = std::stof(components.at(2)),
            z = std::stof(components.at(3));

        return glm::vec3{x, y, z};
    }

    /// Reads .obj formatted 2 element vertex data
    /// @param line A line containing a 2 element vertex
    /// @returns A 2 element GLM vector
    /// @throws std::out_of_range if there are less than 2 elements in the line (from std::vector::at)
    /// @throws std::invalid_argument if an element is not a float (from std::string:stof)
    static glm::vec2 ReadVertex2(const std::string& line)
    {
        auto components = Tokenize(line, ' ');

        auto x = std::stof(components.at(1)),
            y = std::stof(components.at(2));

        return glm::vec2{x, y};
    }

    /// Reads a .obj "f" (face) declaration
    /// @param line A line containing a .obj face
    /// @warning Doesn't return anything, but modifies a bunch of class state
    void OBJModel::OBJMesh::ReadIndex(const std::string& line)
    {
        auto parseComponent = [] (const std::string& component) {
            auto components = Tokenize(component, '/');

            Index idx;
            idx.vertex = static_cast<unsigned>(std::stoul(components[0])) - 1;
            idx.uv = 0;
            idx.normal = 0;

            if (components.size() > 1) {
                idx.uv = static_cast<unsigned>(std::stoul(components[1])) - 1;

                if (components.size() > 2) {
                    idx.normal = static_cast<unsigned>(std::stoul(components[2])) - 1;
                }
            }

            return idx;
        };
        auto components = Tokenize(line, ' ');

        m_indices.emplace_back(parseComponent(components[1]));
        m_indices.emplace_back(parseComponent(components[2]));
        m_indices.emplace_back(parseComponent(components[3]));
    }

    OBJModel::OBJMesh::OBJMesh(
            const std::vector<std::string>& lines
            , std::vector<glm::vec3>& vertices
            , std::vector<glm::vec3>& normals
            , std::vector<glm::vec2>& uvs
    ) :   m_vertices(vertices)
        , m_normals(normals)
        , m_uvs(uvs)
    {
        for (const auto& line : lines) {
            try {
                auto cmds = Tokenize(line, ' ');
                auto mainCmd = cmds.at(0);

                if (mainCmd == "#") {
                    continue;
                } else if (mainCmd == "v") {
                    m_vertices.emplace_back(ReadVertex3(line));
                } else if (mainCmd == "vn") {
                    m_normals.emplace_back(ReadVertex3(line));
                    m_hasNormals = true;
                } else if (mainCmd == "vt") {
                    m_uvs.emplace_back(ReadVertex2(line));
                    m_hasUVs = true;
                } else if (mainCmd == "f") {
                    ReadIndex(line);
                } else if (mainCmd == "usemtl") {
                    m_material = cmds.at(1);
                } else if (mainCmd == "o") {
                    break;
                }
            } catch (const std::out_of_range& ex) {
                // nothing, actually
            } catch (const std::invalid_argument& ex) {
                // also nothing
            }
        }
    }

    /// Generates smoothed normals for a .obj mesh that doesn't have them
    void OBJModel::OBJMesh::GenerateNormals()
    {
        m_normals.resize(m_vertices.size());

        for (auto& index : m_indices) {
            m_normals[index.vertex] = {0.0f, 0.0f, 0.0f};
            index.normal = index.vertex;
        }

        for (size_t i = 2; i < m_indices.size(); i += 3) {
            auto c = m_indices[i],
                    b = m_indices[i - 1],
                    a = m_indices[i - 2];

            auto ab = m_vertices[b.vertex] - m_vertices[a.vertex];
            auto ac = m_vertices[c.vertex] - m_vertices[a.vertex];

            auto cross = glm::cross(ab, ac);

            m_normals[a.normal] += cross;
            m_normals[b.normal] += cross;
            m_normals[c.normal] += cross;
        }

        for (auto& normal : m_normals) {
            normal = glm::normalize(normal);
        }

        m_hasNormals = true;
    }

    /// Prepares a OBJModel for uploading
    /// @returns A mesh in the GPU, containing the data in the obj model
    Engine::GL::Mesh OBJModel::OBJMesh::Upload(GL::Texture* diffuse, GL::Texture* specular, GL::Texture* bump)
    {
        if (!m_hasNormals) {
            GenerateNormals();
        }

        size_t idx = 0;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;

        std::vector<unsigned> indices;

        auto hasher = [] (const Index& idx) {
            using std::hash;

            size_t res = 17;
            res = res * 31 + hash<unsigned>()(idx.vertex);
            res = res * 31 + hash<unsigned>()(idx.normal);
            res = res * 31 + hash<unsigned>()(idx.uv);

            return res;
        };

        std::unordered_map<Index, size_t, decltype(hasher)> indexMap(1000, hasher);

        for (const auto& index : m_indices) {
            if (auto it = indexMap.find(index); it == indexMap.end()) {
                vertices.push_back(m_vertices[index.vertex]);

                auto normal = m_hasNormals ? m_normals[index.normal] : glm::vec3(0.0f, 0.0f, 0.0f);

                normals.push_back(normal);

                if (m_hasUVs) {
                    uvs.push_back(m_uvs[index.uv]);
                }

                indices.push_back(idx);
                indexMap[index] = idx;
                idx++;
            } else {
                indices.push_back(it->second);
            }
        }

        return Engine::GL::Mesh(vertices, uvs, normals, indices, diffuse, specular, bump);
    }

    void OBJModel::ParseMtl(const std::string& path)
    {
        auto raw = FS::ReadAllBytes(path.c_str());

        auto str = std::string{raw.begin(), raw.end()};

        auto lines = Tokenize(str, '\n');

        auto& materials = m_materials;

        for (const auto& line : lines) {
            try {
                auto cmds = Tokenize(line, ' ');
                auto mainCmd = cmds.at(0);

                if (mainCmd == "newmtl") {
                    materials.emplace_back(Material(cmds.at(1)));
                } else if (mainCmd == "Ns") {
                    materials.back().specularPower = std::stof(cmds.at(1));
                } else if (mainCmd == "map_Kd") {
                    materials.back().diffuseMap = cmds.at(1);
                } else if (mainCmd == "map_Ks") {
                    materials.back().specularMap = cmds.at(1);
                } else if (mainCmd == "map_Bump") {
                    materials.back().bumpMap = cmds.at(1);
                }
            } catch (const std::out_of_range& ex) {
            }
        }


    }

    /// Loads a .obj file
    /// @param fileData A string containing the file's text
    /// @returns A mesh, representing the data contained in the text
    OBJModel::OBJModel(const std::string& fileData)
    {
        auto lines = Tokenize(fileData, '\n');

        for (size_t i = 0; i < lines.size(); ++i) {
            auto& line = lines[i];
            // TODO: improve this so it skips stuff sent to OBJMesh. right now it's kind of a waste
            try {
                auto cmds = Tokenize(line, ' ');
                auto mainCmd = cmds.at(0);

                if (mainCmd == "mtllib") {
                    ParseMtl(cmds.at(1));
                } else if (mainCmd == "o") {
                    // seek to next object
                    size_t j = i + 1;

                    for (; j < lines.size(); ++j) {
                        auto cmds = Tokenize(lines[j], ' ');

                        if (cmds.at(0) == "o") {
                            break;
                        }
                    }

                    auto spliced = std::vector(lines.data() + i + 1, lines.data() + j);

                    m_meshes.emplace_back(OBJMesh{spliced, m_vertices, m_normals, m_uvs});
                }
            } catch (const std::out_of_range& ex) {
                // nada
            }
        }
    }

    Engine::GL::Model OBJModel::Upload()
    {
        // stage 1: texture upload
        for (const auto& mesh : m_meshes) {
            if (mesh.m_material.empty()) {
                continue;
            }

            auto& mtlName = mesh.m_material;
            auto search = std::find_if(m_materials.begin(), m_materials.end(), [mtlName](const Material& mtl) {
                return mtl.name == mtlName;
            });

            if (search == m_materials.end()) {
                throw std::runtime_error("mesh referenced missing material");
            }

            auto mtl = *search;

            if (!mtl.diffuseMap.empty()) {
                // amazing language.
                __textures.try_emplace(
                        mtl.diffuseMap,
                        std::move(GL::Texture{mtl.diffuseMap.c_str()})
                );
            }

            if (!mtl.specularMap.empty()) {
                __textures.try_emplace(
                        mtl.specularMap,
                        std::move(GL::Texture{mtl.diffuseMap.c_str()})
                );
            }

            if (!mtl.bumpMap.empty()) {
                __textures.try_emplace(
                        mtl.bumpMap,
                        std::move(GL::Texture{mtl.bumpMap.c_str()})
                );
            }
        }

        std::vector<GL::Mesh> uploaded;
        // stage 2: mesh upload
        for (auto& mesh : m_meshes) {
            GL::Texture* diffuse = nullptr;
            GL::Texture* specular = nullptr;
            GL::Texture* bump = nullptr;
            if (!mesh.m_material.empty()) {
                auto& mtlName = mesh.m_material;
                auto search = std::find_if(m_materials.begin(), m_materials.end(), [mtlName](const Material& mtl) {
                    return mtl.name == mtlName;
                });

                auto mtl = *search;

                auto find = __textures.find(mtl.diffuseMap);

                if (find != __textures.end()) {
                    diffuse = &find->second;
                }

                find = __textures.find(mtl.specularMap);

                if (find != __textures.end()) {
                    specular = &find->second;
                }

                find = __textures.find(mtl.bumpMap);

                if (find != __textures.end()) {
                    bump = &find->second;
                }
            }

            uploaded.emplace_back(mesh.Upload(diffuse, specular, bump));
        }

        return GL::Model(std::move(uploaded));
    }

}