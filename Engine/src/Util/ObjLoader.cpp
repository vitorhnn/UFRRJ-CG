/// @file
/// Contains all the .obj file loader functions
/// @author Victor Hermann "vitorhnn" Chiletto

#include "Util/ObjLoader.hpp"

#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>

/// Container namespace for the .obj loader
namespace Engine::Util::ObjLoader {
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
    void OBJModel::ReadIndex(const std::string& line)
    {
        auto parseComponent = [] (const std::string& component) {
            auto components = Tokenize(component, '/');

            Index idx;
            idx.vertex = static_cast<unsigned>(std::stoul(components[0]));
            idx.uv = 0;
            idx.normal = 0;

            if (components.size() > 1) {
                idx.uv = static_cast<unsigned>(std::stoul(components[1]));

                if (components.size() > 2) {
                    idx.normal = static_cast<unsigned>(std::stoul(components[2]));
                }
            }

            return idx;
        };
        auto components = Tokenize(line, ' ');

        m_indices.emplace_back(parseComponent(components[1]));
        m_indices.emplace_back(parseComponent(components[2]));
        m_indices.emplace_back(parseComponent(components[3]));
    }

    /// Loads a .obj file
    /// @param fileData A string containing the file's text
    /// @returns A mesh, representing the data contained in the text
    OBJModel::OBJModel(const std::string& fileData)
    {
        auto lines = Tokenize(fileData, '\n');

        for (const auto& line : lines) {
            try {
                switch (line.at(0)) {
                    case 'v': {
                        switch (line.at(1)) {
                            [[fallthrough]]
                            case ' ':
                            case '\t':
                                m_vertices.emplace_back(ReadVertex3(line));
                                break;
                            case 'n':
                                m_normals.emplace_back(ReadVertex3(line));
                                m_hasNormals = true;
                                break;
                            case 't':
                                m_uvs.emplace_back(ReadVertex2(line));
                                m_hasUVs = true;
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    case 'f': {
                        ReadIndex(line);
                        break;
                    }
                    default:
                        break;
                }
            } catch (const std::out_of_range& ex) {
                // nothing, actually
            } catch (const std::invalid_argument& ex) {
                // also nothing
            }
        }

        puts("done, I guess");
    }

    /// Prepares a OBJModel for uploading
    /// @returns A mesh in the GPU, containing the data in the obj model
    Engine::GL::Mesh OBJModel::Upload()
    {
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

        std::unordered_map<Index, size_t, decltype(hasher)> indexMap(10, hasher);

        for (const auto& index : m_indices) {
            if (auto it = indexMap.find(index); it == indexMap.end()) {
                vertices.push_back(m_vertices[index.vertex - 1]);

                auto normal = m_hasNormals ? m_normals[index.normal - 1] : glm::vec3(0.0f, 0.0f, 0.0f);

                normals.push_back(normal);

                if (m_hasUVs) {
                    uvs.push_back(m_uvs[index.uv - 1]);
                }

                indices.push_back(idx);
                indexMap[index] = idx;
                idx++;
            } else {
                indices.push_back(it->second);
            }
        }

        if (!m_hasNormals) {
            // unsmoothed normal calculation
            for (size_t i = 0; i < indices.size(); i += 3) {
                auto a = indices[i],
                     b = indices[i + 1],
                     c = indices[i + 2];

                auto ab = vertices[b] - vertices[a];
                auto ac = vertices[c] - vertices[a];

                auto cross = glm::cross(ab, ac);

                normals[a] += cross;
                normals[b] += cross;
                normals[c] += cross;
            }

            for (auto& normal : normals) {
                normal = glm::normalize(normal);
            }
        }

        return Engine::GL::Mesh(vertices, uvs, normals, indices);
    }
}