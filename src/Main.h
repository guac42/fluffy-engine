#ifndef PATHTRACER_MAIN_H
#define PATHTRACER_MAIN_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gfx/Window.h"
#include "Camera.h"
#include "rendering/Skybox.h"
#include "rendering/Text.h"
#include "rendering/objects/ShaderProgram.h"
#include "rendering/objects/VertexArray.h"
#include "World.h"
#include "Client.h"
#include "Config.h"
#include "Ui.h"

class GameWindow : public Window {
public:
    Program* program;
    VertexArray* vao;
    Skybox* skybox;
    World* world;
    Client* client;
    Text* text;

    GameWindow()
    : Window("Fluffy Engine") {

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

        world = new World();

        client = new Client(this, world);
        client->setPosition(glm::vec3(0.f, 1.f, 0.f));

        world->addThing(client);

        text = new Text(client);
        text->initFont();

        Ui::InitImGui(this);

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
        Ui::preUpdateFrame();
        this->client->updateFrame();
        this->client->updateGui();

        // Updated after physics objects
        this->world->updateWorld(this->deltaTime());
        // Updated after gui implementations
        Ui::postUpdateFrame();
        Window::OnUpdateFrame();
    }

    void OnRenderFrame() override {
        Window::OnRenderFrame();
        //Child implementation
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->skybox->Render(this->client->getProjection(), this->client->getView());
        program->Use();
        program->Upload("m", glm::mat4(1));
        program->Upload("v", this->client->getView());
        program->Upload("p", this->client->getProjection());
        vao->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
#ifdef DEBUG
        this->world->renderDebug(this->client);
#endif
        text->display(30, 30, glm::vec3(0), "Pee Pee");
        Ui::render();
    }

    void OnGameTick() override {
        Window::OnGameTick();
        // Child implementation
    }

    void OnResize() override {
        client->resize();
    }

    void OnKeyPress(int key, int action) override {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                if (action != GLFW_PRESS) break;
                Window::ToggleCursorLock(); // Updates Window::cursorLocked
                Ui::visible(!Window::cursorLocked); // If cursor is NOT locked show Ui
                break;
        }
    }
};

#endif //PATHTRACER_MAIN_
