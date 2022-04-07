#ifndef PATHTRACER_MAIN_H
#define PATHTRACER_MAIN_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gfx/Window.h"
#include "Camera.h"
#include "Skybox.h"
#include "objects/ShaderProgram.h"
#include "objects/VertexArray.h"
#include "World.h"

class GameWindow : public Window {
public:
    glm::mat4 projection;
    Program *program;
    VertexArray *vao;
    Skybox *skybox;
    World *world;
    Client *client;

    GameWindow()
    : Window("Game Frame") {
        this->projection = glm::perspective(glm::radians(90.0f), (float)this->width/(float)this->height, 0.01f, 100.0f);
    }

    void OnLoad() override {
        glEnable(GL_DEPTH_TEST);

        {
            float data[]{
                    -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                    0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            };

            Buffer dataBuffer;
            dataBuffer.Allocate(sizeof(data), data, GL_DYNAMIC_STORAGE_BIT);

            vao = new VertexArray();
            vao->AddSourceBuffer(dataBuffer, 0, 6*sizeof(float));
            vao->AddSourceBuffer(dataBuffer, 1, 6*sizeof(float), 3*sizeof(float));
            vao->SetAttribFormat(0, 3, GL_FLOAT);
            vao->SetAttribFormat(1, 3, GL_FLOAT);
        }

        skybox = new Skybox("./resources/skybox.jpg");

        client = new Client(glm::vec3(0.f, 1.f, 0.f));

        world = new World();
        world->addPlayer(client);

        program = new Program({new Shader(Shader::Vertex, "../resources/solid.vert"),
                               new Shader(Shader::Fragment, "../resources/solid.frag")});
    }

    void OnClose() override {
        delete program;
        program = nullptr;
        delete vao;
        vao = nullptr;
        delete world;
    }

    void OnUpdateFrame() override {
        this->client->updateInputs(this);
        this->world->updateWorld(this->deltaTime());
        Window::OnUpdateFrame();
    }

    void OnRenderFrame() override {
        Window::OnRenderFrame();
        //Child implementation
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program->Use();
        program->Upload("m", glm::mat4(1));
        program->Upload("v", this->client->getView());
        program->Upload("p", this->projection);
        vao->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        this->skybox->Render(this->projection, this->client->getView());
    }

    void OnGameTick() override {
        Window::OnGameTick();
        //Child implementation
    }

    void OnResize() override {
        this->projection = glm::perspective(glm::radians(90.0f), (float)this->width/(float)this->height, 0.01f, 100.0f);
    }

    void OnKeyPress() override {

    }
};

#endif //PATHTRACER_MAIN_
