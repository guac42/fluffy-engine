#ifndef GAMEFRAME_THING_H
#define GAMEFRAME_THING_H

#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

class Thing {
protected:
    glm::vec3 position;
    // Physics stuff
    btCollisionShape *collisionShape;
    btRigidBody *rigidBody;
    // Rendering stuff
    VertexArray *vao;
    Program *program;

public:
    virtual void initializeBody() = 0;

    /**
     * Update the position, etc.
     */
    virtual void updateTransform() {
        btTransform transform;
        this->rigidBody->getMotionState()->getWorldTransform(transform);
        this->position.x = transform.getOrigin().getX();
        this->position.y = transform.getOrigin().getY();
        this->position.z = transform.getOrigin().getZ();
    }

    btRigidBody* getBody() {
        return this->rigidBody;
    }

    ~Thing() {
        delete rigidBody->getMotionState();
        delete collisionShape;
        delete rigidBody;
        delete vao;
    }
};

#endif //GAMEFRAME_THING_H
