#pragma once

#include "Video/Mesh.hpp"
#include <vector>

namespace Engine::GL {
    class Model {
        std::vector<Mesh> m_meshes;
    public:
        explicit Model(std::vector<Mesh> meshes);

        void Draw();
    };
}