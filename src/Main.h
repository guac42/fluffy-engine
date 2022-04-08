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
#include "Ui.h"

class GameWindow : public Window {
public:
    Program* program;
    VertexArray* vao;
    Skybox* skybox;
    World* world;
    Client* client;
    Ui* ui;

    GameWindow()
    : Window("Game Frame") {

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
            vao->SetAttribFormat(0, 3, GL_FLOAT); // position
            vao->SetAttribFormat(1, 3, GL_FLOAT); // color
        }

        skybox = new Skybox("./resources/blue.png");

        client = new Client(glm::vec3(0.f, 1.f, 0.f));
        client->resize(this);

        world = new World();
        world->addThing(client);

        Ui::InitImGui(Window::handle);
        ui = new Ui();

        program = new Program({new Shader(Shader::Vertex, "../resources/solid.vert"),
                               new Shader(Shader::Fragment, "../resources/solid.frag")});
    }

    void OnClose() override {
        Ui::cleanImGui();
        delete world;
        world = nullptr;
        delete program;
        program = nullptr;
        delete vao;
        vao = nullptr;
    }

    void OnUpdateFrame() override {
        this->ui->updateFrame(this);
        this->client->updateFrame(this);

        // Updated last
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
        program->Upload("p", this->client->getProjection());
        vao->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
#ifdef DEBUG
        //this->world->renderDebug(this->client);
#endif
        this->skybox->Render(this->client->getProjection(), this->client->getView());
        this->ui->render();
    }

    void OnGameTick() override {
        Window::OnGameTick();
        // Child implementation
    }

    void OnResize() override {
        client->resize(this);
    }

    void OnKeyPress(int key, int action) override {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                if (action != GLFW_PRESS) break;
                Window::ToggleCursorLock(); // Updates Window::cursorLocked
                this->ui->visible(!Window::cursorLocked); // If cursor is NOT locked show Ui
                break;
        }
    }
};

#endif //PATHTRACER_MAIN_
