#include "SDL.h"
#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

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

    auto mouseLock = true;
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(mouseLock));

    glEnable(GL_DEPTH_TEST);

    GL::Program prog;
    prog.AttachShader("GLSL/simple_mesh.vert", GL::ShaderType::Vertex);
    prog.AttachShader("GLSL/simple_mesh.frag", GL::ShaderType::Fragment);
    prog.Link();

    GL::Program lampProgram;
    lampProgram.AttachShader("GLSL/simple_mesh.vert", GL::ShaderType::Vertex);
    lampProgram.AttachShader("GLSL/fullbright.frag", GL::ShaderType::Fragment);
    lampProgram.Link();

    auto data = Util::FS::ReadAllBytes("nanosuit.obj");
    data.push_back('\0'); // had some weird issues with null terminators, push one just for safety sake
    auto nanosuit = Util::ObjLoader::OBJModel(std::string{data.begin(), data.end()}).Upload();

    data = Util::FS::ReadAllBytes("cube.obj");
    data.push_back('\0');
    auto cube = Util::ObjLoader::OBJModel(std::string{data.begin(), data.end()}).Upload();


    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    auto model = glm::mat4(1.0f);
    model = glm::scale(model, {0.5f, 0.5f, 0.5f});

    auto invModel = glm::inverseTranspose(glm::mat3(model));

    auto camera = FlyCamera();

    auto lightPos = glm::vec3(5.0f, 0.0f, 0.0f);

    while (!ipt.IsQuitRequested()) {
        ipt.Update();

        if (ipt.ConsumeKey(Input::Keys::F1)) {
            mouseLock = !mouseLock;
            SDL_SetRelativeMouseMode(static_cast<SDL_bool>(mouseLock));
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prog.Use();
        prog.SetUniform("projection", projection);
        prog.SetUniform("objColor", glm::vec3{0.3f, 0.6f, 0.1f});
        prog.SetUniform("lightColor", glm::vec3{1.0f, 1.0f, 1.0f});
        prog.SetUniform("lightPos", lightPos);
        prog.SetUniform("viewPos", camera.GetPosition());

        camera.Update(ipt);
        prog.SetUniform("view", camera.GetViewMatrix());

        auto rotmodel = glm::rotate(model, SDL_GetTicks() / 2000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        prog.SetUniform("model", rotmodel);

        invModel = glm::inverseTranspose(glm::mat3(rotmodel));
        prog.SetUniform("invModel", invModel);

        nanosuit.Draw();

        lampProgram.Use();
        lampProgram.SetUniform("projection", projection);
        lampProgram.SetUniform("view", camera.GetViewMatrix());
        auto cubeModel = glm::translate(glm::mat4(1), lightPos);
        cubeModel = glm::scale(cubeModel, {0.5f, 0.5f, 0.5f});
        lampProgram.SetUniform("model", cubeModel);

        cube.Draw();


        w.Present();
    }

    return 0;
}
