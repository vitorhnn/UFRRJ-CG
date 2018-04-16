#include "SDL.h"
#include "glad/glad.h"

#include "Input/SDLInput.hpp"
#include "Video/Window.hpp"
#include "Video/Program.hpp"

#include "Util/FS.hpp"
#include "Util/ObjLoader.hpp"

/// @mainpage
///
/// Trabalho da matéria de Computação Gráfica da UFRRJ (2018.1)
///

int main()
{
    using namespace Engine;

    SDL_Init(SDL_INIT_EVERYTHING);

    GL::Window w("ufrrj", 1280, 720, false);
    Input::SDLInput ipt;

    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GL::Program prog;
    prog.AttachShader("GLSL/tris.vert", GL::ShaderType::Vertex);
    prog.AttachShader("GLSL/tris.frag", GL::ShaderType::Fragment);
    prog.Link();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    prog.Use();

    auto data = Engine::Util::FS::ReadAllBytes("bosta.obj");
    Engine::Util::ObjLoader::Load(std::string{data.begin(), data.end()});

    while (!ipt.IsQuitRequested()) {
        ipt.Update();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        w.Present();
    }

    return 0;
}
