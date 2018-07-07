#include "SDL.h"
#include "glad/glad.h"
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <assimp/DefaultLogger.hpp>

#include "Input/SDLInput.hpp"
#include "Video/Window.hpp"
#include "Video/Program.hpp"
#include "Video/FlyCamera.hpp"

#include "Util/FS.hpp"
#include "Util/ObjLoader.hpp"
#include "Util/AssimpLoader.hpp"

/// @mainpage
///
/// Trabalho da matéria de Computação Gráfica da UFRRJ (2018.1)
///

int main()
{
    using namespace Engine;

    SDL_Init(SDL_INIT_EVERYTHING);

    Assimp::DefaultLogger::create("", Assimp::Logger::LogSeverity::VERBOSE, aiDefaultLogStream_STDERR);

    GL::Window w("ufrrj", 1920, 1080, true, 4);
    Input::SDLInput ipt;

    auto mouseLock = true;
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(mouseLock));

    glEnable(GL_DEPTH_TEST);

    GL::Program prog;
    prog.AttachShader("GLSL/bumpmapped_mesh.vert", GL::ShaderType::Vertex);
    prog.AttachShader("GLSL/bumpmapped_mesh.frag", GL::ShaderType::Fragment);
    prog.Link();

    GL::Program prog2;
    prog2.AttachShader("GLSL/simple_mesh.vert", GL::ShaderType::Vertex);
    prog2.AttachShader("GLSL/simple_mesh.frag", GL::ShaderType::Fragment);
    prog2.Link();

    GL::Program *mainProg = &prog;

    GL::Program lampProgram;
    lampProgram.AttachShader("GLSL/simple_mesh.vert", GL::ShaderType::Vertex);
    lampProgram.AttachShader("GLSL/fullbright.frag", GL::ShaderType::Fragment);
    lampProgram.Link();

    auto nanosuit = Util::AssimpLoader::LoadModel("cyborg.obj");

    auto cube = Util::AssimpLoader::LoadModel("cube.obj");


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

        if (ipt.ConsumeKey(Input::Keys::F2)) {
            if (mainProg == &prog) {
                mainProg = &prog2;
            } else {
                mainProg = &prog;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainProg->Use();
        mainProg->SetUniform("projection", projection);
        mainProg->SetUniform("objColor", glm::vec3{0.3f, 0.6f, 0.1f});
        mainProg->SetUniform("lightColor", glm::vec3{1.0f, 1.0f, 1.0f});
        mainProg->SetUniform("lightPos", lightPos);
        mainProg->SetUniform("viewPos", camera.GetPosition());
        mainProg->SetUniform("diffuseMap", 0);
        mainProg->SetUniform("specularMap", 1);
        mainProg->SetUniform("normalMap", 2);

        camera.Update(ipt);
        mainProg->SetUniform("view", camera.GetViewMatrix());

        auto rotmodel = glm::rotate(model, SDL_GetTicks() / 2000.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        mainProg->SetUniform("model", rotmodel);

        invModel = glm::inverseTranspose(glm::mat3(rotmodel));
        mainProg->SetUniform("invModel", invModel);

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
