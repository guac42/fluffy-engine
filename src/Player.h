#ifndef GAMEFRAME_PLAYER_H
#define GAMEFRAME_PLAYER_H

#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>
#include "Thing.h"

class Player : public Thing {
private:
    std::string name;

public:
    Player(const glm::vec3& position) {
        Thing::position = position;
    }

    /**
     * Initialize bodies
     */
    void initializeBody() override {
        this->collisionShape = new btSphereShape(btScalar(0.6));

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(Thing::position.x, Thing::position.y, Thing::position.z));

        btScalar mass(1.f);
        btVector3 localInertia(0, 0, 0);
        this->collisionShape->calculateLocalInertia(mass, localInertia);

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, this->collisionShape, localInertia);
        this->rigidBody = new btRigidBody(rbInfo);
        this->rigidBody->setAngularFactor(btScalar(0.));
        this->rigidBody->setDamping(btScalar(.8), btScalar(0.));

        // Could disable deactivation, but might be less efficient
        //this->rigidBody->setActivationState(DISABLE_DEACTIVATION);
    }
};

#endif //GAMEFRAME_PLAYER_H
