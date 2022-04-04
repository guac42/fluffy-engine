#ifndef PATHTRACER_CAMERA_H
#define PATHTRACER_CAMERA_H

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gfx/Window.h"

class Camera {
public:
    glm::vec3 Position, Front, Right, Up, Acceleration;
    float MovementSpeed, MouseSensitivity;
    bool moved = false, onGround = false;

    explicit Camera(const glm::vec3& position, float yaw = 90.0f,
           float pitch = 0.0f, float mouseSensitivity = 0.3f, float speed = 2.0f) {
        this->yaw = yaw;
        this->pitch = pitch;

        Front = glm::normalize(glm::vec3(
                std::cos(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch)),
                std::sin(glm::radians(this->pitch)),
                std::sin(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch))
        ));

        Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Front));
        Up = glm::normalize(glm::cross(Front, Right));
        Acceleration = glm::vec3(0);

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

    void processWorldUpdate(float x, float y, float z) {
        this->Position.x = x;
        this->Position.y = y;
        this->Position.z = z;

        this->view = glm::lookAt(Position, Position + Front, Up);
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

        glm::vec3 forward = glm::vec3(Front.x, 0, Front.z),
                right = glm::vec3(Right.x, 0, Right.z);

#define DOWN(x) ((float)game->keyboardManager.isKeyDown(x))

        Acceleration =
                (DOWN(GLFW_KEY_W) * forward) -
                (DOWN(GLFW_KEY_S) * forward) +
                (DOWN(GLFW_KEY_A) * right) -
                (DOWN(GLFW_KEY_D) * right);
        if (Acceleration.x != 0 || Acceleration.z != 0) {
            moved = true;
            Acceleration = 5.f * glm::normalize(Acceleration);
            return;
        }
        moved = false;
    }

private:
    float yaw, pitch;
    glm::mat4 view;
};

#endif //PATHTRACER_CAMERA_H
