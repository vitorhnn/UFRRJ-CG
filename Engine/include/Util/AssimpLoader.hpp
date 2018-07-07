#pragma once

#include "Video/Model.hpp"

namespace Engine::Util::AssimpLoader {
    Engine::GL::Model LoadModel(const char* path);
}