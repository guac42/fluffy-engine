#ifndef GAMEFRAME_PLAYER_H
#define GAMEFRAME_PLAYER_H

#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

class Player {
protected:
    glm::vec3 position;
    // Physics stuff
    btCollisionShape *collisionShape;
    btRigidBody *playerBody{};
    // Rendering stuff

public:
    Player(const glm::vec3& position) :
        position(position) {
    }

    /**
     * Initialize bodies
     */
    void initializeBody() {
        this->collisionShape = new btSphereShape(btScalar(0.6));

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(0, 1, 0));

        btScalar mass(1.f);
        btVector3 localInertia(0, 0, 0);
        this->collisionShape->calculateLocalInertia(mass, localInertia);

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, this->collisionShape, localInertia);
        this->playerBody = new btRigidBody(rbInfo);
        this->playerBody->setAngularFactor(btScalar(0.));
        this->playerBody->setDamping(btScalar(.8), btScalar(0.));

        // Could disable deactivation, but might be less efficient
        //this->cameraBody->setActivationState(DISABLE_DEACTIVATION);
    }

    /**
     * Update the position, etc.
     */
    virtual void update() {
        btTransform transform;
        this->playerBody->getMotionState()->getWorldTransform(transform);
        this->position.x = transform.getOrigin().getX();
        this->position.y = transform.getOrigin().getY();
        this->position.z = transform.getOrigin().getZ();
    }

    btRigidBody* getBody() {
        return this->playerBody;
    }
};

#endif //GAMEFRAME_PLAYER_H
