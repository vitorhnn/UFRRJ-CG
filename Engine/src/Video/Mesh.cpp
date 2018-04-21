#include "Video/Mesh.hpp"


namespace Engine::GL {
    Mesh::Mesh(
              const std::vector<glm::vec3>& pos
            , const std::vector<glm::vec2>& uv
            , const std::vector<glm::vec3>& normal
            , const std::vector<uint32_t>&  index
    ) : m_drawCount(index.size())
    {
        // TODO: std::optional<std::vector<glm::vec3>>> ?
        bool hasUV = !uv.empty();
        bool hasNormal = !normal.empty();

        // generate the vao and the vertex and index buffers
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        // interleave the vertex data
        std::vector<float> vertexData;

        // reserve enough for pos, uv, normal (each has at least 3 floats)
        vertexData.reserve(pos.size() * 3 * 3 * sizeof(float));

        for (size_t i = 0; i < pos.size(); ++i) {
            vertexData.push_back(pos[i].x);
            vertexData.push_back(pos[i].y);
            vertexData.push_back(pos[i].z);

            // uv (gl calls this st for whatever reason)
            if (hasUV) {
                vertexData.push_back(uv[i].s);
                vertexData.push_back(uv[i].t);
            }

            // normals
            if (hasNormal) {
                vertexData.push_back(normal[i].x);
                vertexData.push_back(normal[i].y);
                vertexData.push_back(normal[i].z);
            }
        }

        glBindVertexArray(m_vao);

        // vertex data upload
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

        // index data upload
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(uint32_t), index.data(), GL_STATIC_DRAW);

        // vertex data layout setup

#define STUPID_CAST(x) (reinterpret_cast<GLvoid*>(x))

        // how many bytes to skip between each vertex
        size_t stride = 3 * sizeof(float); // + 2 * sizeof(float) + 3 * sizeof(float);
        if (hasUV) stride += 2 * sizeof(float);
        if (hasNormal) stride += 3 * sizeof(float);

        // how many bytes to skip for the current vertex attribute
        size_t offset = 0;

        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, STUPID_CAST(offset));
        offset += 3 * sizeof(float);

        // uv
        if (hasUV) {
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, STUPID_CAST(offset));
            offset += 2 * sizeof(float);
        }

        // normals
        if (hasNormal) {
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, STUPID_CAST(offset));
            offset += 3 * sizeof(float);
        }

#undef STUPID_CAST

        glBindVertexArray(0);
    }

    void Mesh::Draw()
    {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}