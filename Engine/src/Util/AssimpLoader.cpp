#include "Util/AssimpLoader.hpp"

#include <stdexcept>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

/// So I gave up on ObjLoader, because it's kind of awful and I'm in a rush
/// So yeah. assimp, ass imp. even though I hate its API
namespace Engine::Util::AssimpLoader {
    using namespace Engine::GL;

    std::unordered_map<std::string, Texture> __textures;

    static Texture* LoadTexture(const char *path)
    {
        if (auto search = __textures.find(path); search != __textures.end()) {
            return &search->second;
        } else {
            auto success = __textures.emplace(path, Texture(path));
            return &success.first->second;
        }
    }

    static Mesh ProcessMesh(const aiScene *scene, aiMesh* mesh)
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> tangents;

        std::vector<unsigned> indices;

        Texture *diffuseMap = nullptr;
        Texture *specularMap = nullptr;
        Texture *normalMap = nullptr;

        for (size_t i = 0; i < mesh->mNumVertices; ++i) {
            auto vertex = mesh->mVertices[i];
            vertices.emplace_back(vertex.x, vertex.y, vertex.z);

            auto normal = mesh->mNormals[i];
            normals.emplace_back(normal.x, normal.y, normal.z);

            if (mesh->mTextureCoords[0] != nullptr) {
                auto uv = mesh->mTextureCoords[0][i];
                uvs.emplace_back(uv.x, uv.y);
            }

            if (mesh->mTangents != nullptr) {
                auto tangent = mesh->mTangents[i];
                tangents.emplace_back(tangent.x, tangent.y, tangent.z);
            }
        }

        for (size_t i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];

            for (size_t j = 0; j < face.mNumIndices; ++j) {
                indices.emplace_back(face.mIndices[j]);
            }
        }

        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];

            if (mtl->GetTextureCount(aiTextureType_DIFFUSE) == 1) {
                aiString str;
                mtl->GetTexture(aiTextureType_DIFFUSE, 0, &str);
                diffuseMap = LoadTexture(str.C_Str());
            }

            if (mtl->GetTextureCount(aiTextureType_SPECULAR) == 1) {
                aiString str;
                mtl->GetTexture(aiTextureType_SPECULAR, 0, &str);
                specularMap = LoadTexture(str.C_Str());
            }

            if (mtl->GetTextureCount(aiTextureType_NORMALS) == 1) {
                aiString str;
                mtl->GetTexture(aiTextureType_NORMALS, 0, &str);
                normalMap = LoadTexture(str.C_Str());
            }
        }

        return Mesh(vertices, uvs, normals, tangents, indices, diffuseMap, specularMap, normalMap);
    }

    static void ProcessNode(const aiScene* scene, aiNode* node, std::vector<Mesh>& meshes)
    {
        for (size_t i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.emplace_back(ProcessMesh(scene, mesh));
        }

        for (size_t i = 0; i < node->mNumChildren; ++i) {
            aiNode* child = node->mChildren[i];
            ProcessNode(scene, child, meshes);
        }
    }

    Model LoadModel(const char* path)
    {
        std::vector<Mesh> meshes;
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
                path,
                aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_CalcTangentSpace |
                aiProcess_FlipUVs
        );

        if (scene == nullptr) {
            throw std::runtime_error("assimp import failed");
        }

        ProcessNode(scene, scene->mRootNode, meshes);

        return Model{std::move(meshes)};
    }
}