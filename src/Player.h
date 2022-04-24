#ifndef GAMEFRAME_PLAYER_H
#define GAMEFRAME_PLAYER_H

#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>
#include "Thing.h"

class Player : public Thing {
private:
    std::string name;

protected:
    const float radius = .3f, height = .6f;
    const float verticalOffset = height * .5f + radius;

    /**
     * Initialize bodies
     */
    void initializeBody() override {
        this->pCollisionShape = new btCapsuleShape(radius, height);

        // Default player position
        this->motionTransform.setIdentity();
        this->motionTransform.setOrigin(btVector3(0.f, 1.f, 0.f));

        btScalar mass(1.f);
        btVector3 localInertia(0, 0, 0);
        this->pCollisionShape->calculateLocalInertia(mass, localInertia);

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState *myMotionState = new btDefaultMotionState(this->motionTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, this->pCollisionShape, localInertia);
        rbInfo.m_friction = 0.f;
        rbInfo.m_restitution = 0.f;
        rbInfo.m_linearDamping = 0.f;

        this->pRigidBody = new btRigidBody(rbInfo);
        this->pRigidBody->setAngularFactor(btScalar(0.));
        this->pRigidBody->setActivationState(DISABLE_DEACTIVATION);
    }
};

#endif //GAMEFRAME_PLAYER_H
