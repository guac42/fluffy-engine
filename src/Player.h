#ifndef GAMEFRAME_PLAYER_H
#define GAMEFRAME_PLAYER_H

#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>
#include "Thing.h"

class Player : public Thing {
private:
    const float radius = .3f, height = .6f;
    std::string name;

protected:
    const float verticalOffset = height * .5f + radius;

    /**
     * Initialize bodies
     */
    void initializeBody() override {
        this->collisionShape = new btCapsuleShape(radius, height);

        // Default player position
        this->transform.setIdentity();
        this->transform.setOrigin(btVector3(0.f, 1.f, 0.f));

        btScalar mass(1.f);
        btVector3 localInertia(0, 0, 0);
        this->collisionShape->calculateLocalInertia(mass, localInertia);

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState *myMotionState = new btDefaultMotionState(this->transform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, this->collisionShape, localInertia);
        rbInfo.m_friction = 0.7;

        this->rigidBody = new btRigidBody(rbInfo);
        this->rigidBody->setAngularFactor(btScalar(0.));

        // Could disable deactivation, but might be less efficient
        //this->rigidBody->setActivationState(DISABLE_DEACTIVATION);
    }
};

#endif //GAMEFRAME_PLAYER_H
