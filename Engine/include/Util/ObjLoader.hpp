#pragma once

#include <string>

#include "Video/Mesh.hpp"

namespace Engine::Util::ObjLoader {
    std::vector<std::string> Tokenize(const std::string& str, const char separator);

    Engine::GL::Mesh Load(const std::string& fileData);
}