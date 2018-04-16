/// @file
/// Contains all the .obj file loader functions
/// @author Victor Hermann "vitorhnn" Chiletto

#include "Util/ObjLoader.hpp"

#include <vector>
#include <string>
#include <stdexcept>

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
    static glm::vec3 ReadVertex(const std::string& line)
    {
        auto components = Tokenize(line, ' ');

        auto x = std::stof(components.at(1)),
            y = std::stof(components.at(2)),
            z = std::stof(components.at(3));

        return glm::vec3{x, y, z};
    }

    /// Loads a .obj file
    /// @param fileData A string containing the file's text
    /// @returns A mesh, representing the data contained in the text
    Engine::GL::Mesh Load(const std::string& fileData)
    {
        std::vector<glm::vec3> vertices;

        auto lines = Tokenize(fileData, '\n');

        for (const auto& line : lines) {
            try {
                switch (line.at(0)) {
                    case 'v': {
                        switch (line.at(1)) {
                            [[fallthrough]]
                            case ' ':
                            case '\t':
                                vertices.emplace_back(ReadVertex(line));
                            default:
                                break;
                        }
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

        for (const auto& vertex : vertices) {
            printf("(%f, %f, %f)\n", vertex.x, vertex.y, vertex.z);
        }
    }
}