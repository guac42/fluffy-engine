#ifndef PATHTRACER_CAMERA_H
#define PATHTRACER_CAMERA_H

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gfx/Window.h"

class Camera {
protected:
    glm::vec3 front, right, up;
    float mouseSensitivity = 0.3f;

    explicit Camera(const glm::vec3& position, float yaw = 90.0f, float pitch = 0.0f) {
        this->yaw = yaw;
        this->pitch = pitch;

        this->front = glm::normalize(glm::vec3(
                std::cos(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch)),
                std::sin(glm::radians(this->pitch)),
                std::sin(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch))
        ));

        this->right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->front));
        this->up = glm::normalize(glm::cross(this->front, this->right));

        this->view = glm::lookAt(position, position + this->front, this->up);
    }

    /**
     * Update view matrix
     * @param position position of camera
     */
    void updateView(const glm::vec3& position) {
        this->view = glm::lookAt(position, position + front, up);
    }

    virtual void updateFrame(Window *game) {
        glm::vec2 mouseDelta = game->mouseManager.delta;
        yaw += mouseDelta.x * mouseSensitivity;
        pitch -= mouseDelta.y * mouseSensitivity;

        if (pitch >= 90) pitch = 89.999f;
        if (pitch <= -90) pitch = -89.999f;

        this->front = glm::normalize(glm::vec3(
                cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                sin(glm::radians(this->pitch)),
                sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))
        ));

        this->right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->front));
        this->up = glm::normalize(glm::cross(this->front, this->right));
    }

    void resize(int width, int height) {
        this->projection = glm::perspective(glm::radians(90.0f), (float)width/(float)height, 0.01f, 100.0f);
    }

public:
    inline glm::mat4 getView() const {
        return this->view;
    }

    inline glm::mat4 getProjection() const {
        return this->projection;
    }

    inline float getYaw() const {
        return this->yaw;
    }

    inline float getPitch() const {
        return this->pitch;
    }

private:
    float yaw, pitch;
    glm::mat4 view, projection;
};

#endif //PATHTRACER_CAMERA_H
