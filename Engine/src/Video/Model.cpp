#include "Video/Model.hpp"

namespace Engine::GL {
    Model::Model(std::vector<Engine::GL::Mesh> meshes) :
        m_meshes(std::move(meshes))
    {
    }

    void Model::Draw()
    {
        for (auto& mesh : m_meshes) {
            mesh.Draw();
        }
    }
}