#ifndef PATHTRACER_CAMERA_H
#define PATHTRACER_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gfx/Window.h"

class Camera {
public:
    glm::vec3 Position, Front, Right, Up, Velocity;
    float MovementSpeed, MouseSensitivity;

    explicit Camera(const glm::vec3& position, float yaw = 90.0f,
           float pitch = 0.0f, float mouseSensitivity = 0.3f, float speed = 2.0f) {
        this->yaw = yaw;
        this->pitch = pitch;

        Front = glm::normalize(glm::vec3(
                cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                sin(glm::radians(this->pitch)),
                sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))
        ));

        Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Front));
        Up = glm::normalize(glm::cross(Front, Right));

        this->view = glm::lookAt(position, position + Front, Up);
        Position = position;
        MovementSpeed = speed;
        MouseSensitivity = mouseSensitivity;
    }

    inline glm::mat4 getView() const {
        return this->view;
    }

    inline float getYaw() const {
        return this->yaw;
    }

    inline float getPitch() const {
        return this->pitch;
    }

    void processFrameUpdate(Window *game, bool& frameChanged) {
        frameChanged = false;

        glm::vec2 mouseDelta = game->mouseManager.delta;
        yaw += mouseDelta.x * MouseSensitivity;
        pitch -= mouseDelta.y * MouseSensitivity;

        if (pitch >= 90) pitch = 89.999f;
        if (pitch <= -90) pitch = -89.999f;

        Front = glm::normalize(glm::vec3(
                cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                sin(glm::radians(this->pitch)),
                sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))
                ));

        Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Front));
        Up = glm::normalize(glm::cross(Front, Right));

        glm::vec3 acceleration =
                ((float)game->keyboardManager.isKeyDown(GLFW_KEY_W) * Front) -
                ((float)game->keyboardManager.isKeyDown(GLFW_KEY_S) * Front) +
                ((float)game->keyboardManager.isKeyDown(GLFW_KEY_A) * Right) -
                ((float)game->keyboardManager.isKeyDown(GLFW_KEY_D) * Right) +
                ((float)game->keyboardManager.isKeyDown(GLFW_KEY_SPACE) * Up) -
                ((float)game->keyboardManager.isKeyDown(GLFW_KEY_LEFT_CONTROL) * Up);
        Velocity +=
                game->keyboardManager.isKeyDown(GLFW_KEY_LEFT_SHIFT) ? acceleration * 5.0f : acceleration;

        frameChanged = mouseDelta.x != 0 || mouseDelta.y != 0 || acceleration != glm::vec3(0) || Velocity != glm::vec3(0);

        // Velocity cut off
        Velocity *= (float)(glm::dot(Velocity, Velocity) >= 0.01f) * 0.95f;
        Velocity += game->deltaTime() * acceleration;
        Position += game->deltaTime() * (float)(glm::length(Velocity) >= 0.0001f) * Velocity;
        this->view = glm::lookAt(Position, Position + Front, Up);
    }

private:
    float yaw, pitch;
    glm::mat4 view;
};

#endif //PATHTRACER_CAMERA_H
