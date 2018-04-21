#include "SDL.h"
#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Input/SDLInput.hpp"
#include "Video/Window.hpp"
#include "Video/Program.hpp"
#include "Video/FlyCamera.hpp"

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

    SDL_SetRelativeMouseMode(SDL_TRUE);
    glEnable(GL_DEPTH_TEST);

    GL::Program prog;
    prog.AttachShader("GLSL/simple_mesh.vert", GL::ShaderType::Vertex);
    prog.AttachShader("GLSL/simple_mesh.frag", GL::ShaderType::Fragment);
    prog.Link();
    prog.Use();

    auto data = Util::FS::ReadAllBytes("bosta.obj");
    auto mesh = Util::ObjLoader::OBJModel(std::string{data.begin(), data.end()}).Upload();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
    prog.SetUniform("projection", projection);
    glm::mat4 model;
    model = glm::scale(model, {0.5f, 0.5f, 0.5f});
    prog.SetUniform("model", model);

    auto camera = FlyCamera();

    while (!ipt.IsQuitRequested()) {
        ipt.Update();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Update(ipt);
        prog.SetUniform("view", camera.GetViewMatrix());

        mesh.Draw();

        w.Present();
    }

    return 0;
}
