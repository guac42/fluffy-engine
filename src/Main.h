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
    Camera *camera = new Camera(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection;
    Program *program;
    VertexArray *vao;
    Skybox *skybox;
    World *world;

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

        world = new World(this->camera);

        program = new Program({new Shader(Shader::Vertex, "#version 430 core\n"
                                                           "layout (location = 0) in vec3 aPosition;\n"
                                                           "layout (location = 1) in vec3 aColor;\n"
                                                           "uniform mat4 m, v, p;\n"
                                                           "out vec3 color;\n"
                                                           "void main() {\n"
                                                           "    color = aColor;\n"
                                                           "    gl_Position = p * v * m * vec4(aPosition, 1.0f);\n"
                                                           "}", false),
                               new Shader(Shader::Fragment, "#version 430 core\n"
                                                             "in vec3 color;\n"
                                                             "out vec4 frag_color;\n"
                                                             "void main() {\n"
                                                             "    frag_color = vec4(color, 1.0f);\n"
                                                             "}", false)});
    }

    void OnClose() override {
        delete program;
        program = nullptr;
        delete vao;
        vao = nullptr;
        delete world;
    }

    void OnUpdateFrame() override {
        bool frameChanged;
        this->camera->processFrameUpdate(this, frameChanged);
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
        program->Upload("v", this->camera->getView());
        program->Upload("p", this->projection);
        vao->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        this->skybox->Render(this->projection, this->camera->getView());
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
