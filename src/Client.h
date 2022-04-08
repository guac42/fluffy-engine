#ifndef GAMEFRAME_FRSTPERSONPLAYER_H
#define GAMEFRAME_FRSTPERSONPLAYER_H

#include "gfx/Window.h"
#include "Player.h"
#include "Camera.h"

class Client : public Player, public Camera {
private:
    btVector3 acceleration;
    const float movementAcc = 5.f, jumpImpulse = 8.f;
    bool moved = false, onGround = false, jumped = false;

    void updateAcceleration(Window *game) {
        btVector3 forward = btVector3(Camera::front.x, 0, Camera::front.z),
                right = btVector3(Camera::right.x, 0, Camera::right.z);

#define IS_DOWN(x) (game->keyboardManager.isKeyDown(x))

        //TODO: should be isDown and check if on ground
        this->jumped = game->keyboardManager.isKeyDown(GLFW_KEY_SPACE);
        //printf("Jumped: %d\n", game->keyboardManager.keys[GLFW_KEY_SPACE] ^ 0x00000010);
        this->acceleration =
                ((float)IS_DOWN(GLFW_KEY_W) * forward) -
                ((float)IS_DOWN(GLFW_KEY_S) * forward) +
                ((float)IS_DOWN(GLFW_KEY_A) * right) -
                ((float)IS_DOWN(GLFW_KEY_D) * right);
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
     * Update camera look-vectors
     * @param window to capture inputs
     */
    void updateFrame(Window *window) override {
        if (!window->cursorLocked) return;   // Don't update if in Ui
        Camera::updateFrame(window);
        this->updateAcceleration(window);
        if (this->moved) {
            this->rigidBody->activate();
            this->rigidBody->applyCentralForce(this->acceleration);
        }
        if (this->jumped) {
            this->rigidBody->activate();
            this->rigidBody->applyCentralImpulse(btVector3(0.f, this->jumpImpulse, 0.f));
        }
    }

    /**
     * Update the view Matrix
     * Call after world is updated
     */
    void updateTransform() override {
        Thing::updateTransform();
        Camera::updateView(this->position);
    }

    void resize(Window* window) {
        Camera::resize(window->width, window->height);
    }
};

#endif //GAMEFRAME_FRSTPERSONPLAYER_H
