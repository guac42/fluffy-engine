#ifndef GAMEFRAME_FRSTPERSONPLAYER_H
#define GAMEFRAME_FRSTPERSONPLAYER_H

#include "gfx/Window.h"
#include "Player.h"
#include "Camera.h"

class Client : public Player, public Camera {
private:
    btVector3 acceleration;
    const float movementAcc = 5.f;
    bool moved = false, onGround = false;

    void updateAcceleration(Window *game) {
        btVector3 forward = btVector3(Camera::front.x, 0, Camera::front.z),
                right = btVector3(Camera::right.x, 0, Camera::right.z);

#define IS_DOWN(x) ((float)game->keyboardManager.isKeyDown(x))

        this->acceleration =
                (IS_DOWN(GLFW_KEY_W) * forward) -
                (IS_DOWN(GLFW_KEY_S) * forward) +
                (IS_DOWN(GLFW_KEY_A) * right) -
                (IS_DOWN(GLFW_KEY_D) * right);
        this->moved = !this->acceleration.isZero();
        if (this->moved)
            this->acceleration = this->movementAcc * this->acceleration.normalize();
    }

public:
    explicit Client(const glm::vec3& position) :
            Camera(position), Player(position) {
        Player::initializeBody();
    }

    /**
     * Update rotation and input
     * Call before world is updated
     *
     * @param window window object for player
     * @param delta
     */
    void updateInputs(Window *window) {
        Camera::update(window);
        this->updateAcceleration(window);
        if (this->moved) {
            this->playerBody->activate();
            this->playerBody->applyCentralForce(this->acceleration);
        }
    }

    /**
     * Update the view Matrix
     * Call after world is updated
     */
    void update() override {
        Player::update();
        Camera::updateView(this->position);
    }
};

#endif //GAMEFRAME_FRSTPERSONPLAYER_H
