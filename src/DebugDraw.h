#ifndef GAMEFRAME_DEBUGDRAW_H
#define GAMEFRAME_DEBUGDRAW_H

#include <bullet/LinearMath/btIDebugDraw.h>

#include <glad/glad.h>

class DebugDraw : public btIDebugDraw {
private:
    Program* program;
    VertexArray* vao;
    int drawMode;
    union point {
        float f[6];
    };
    std::vector<point> points;
    float lineWidth = 2.f;

public:
    DebugDraw() {
        this->program = new Program({new Shader(Shader::Vertex, "../resources/solid.vert"),
                     new Shader(Shader::Fragment, "../resources/solid.frag")});
        this->vao = new VertexArray();
    }

    ~DebugDraw() override {
        delete this->program;
        delete this->vao;
    }

    void render(Camera* camera) {
        if (this->points.empty())
            return;

        Buffer buffer;
        buffer.Allocate(this->points.size()*sizeof(this->points[0]), this->points.data(), GL_DYNAMIC_STORAGE_BIT);

        vao = new VertexArray();
        vao->AddSourceBuffer(buffer, 0, 6*sizeof(float));
        vao->AddSourceBuffer(buffer, 1, 6*sizeof(float), 3*sizeof(float));
        vao->SetAttribFormat(0, 3, GL_FLOAT);
        vao->SetAttribFormat(1, 3, GL_FLOAT);

        program->Use();
        program->Upload("m", glm::mat4(1));
        program->Upload("v", camera->getView());
        program->Upload("p", camera->getProjection());
        vao->Bind();

        glLineWidth(this->lineWidth);
        glDrawArrays(GL_LINES, 0, this->points.size()); // Array of vertex + color

        this->points.clear();
    }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override {
        points.push_back({from.x(), from.y(), from.z(),
                          color.x(), color.y(), color.z()});
        points.push_back({to.x(), to.y(), to.z(),
                          color.x(), color.y(), color.z()});
    }

    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {
        drawLine(PointOnB, PointOnB + normalOnB, color);
    }

    void reportErrorWarning(const char* warningString) override {
        fprintf(stderr, "[WARNING] %s\n", warningString);
    }

    void draw3dText(const btVector3& location, const char* textString) override {

    }

    void setDebugMode(int debugMode) override {
        this->drawMode = debugMode;
    }

    int getDebugMode() const override {
        return this->drawMode;
    }

    void setLineWidth(float width) {
        this->lineWidth = width;
    }
};

#endif //GAMEFRAME_DEBUGDRAW_H
