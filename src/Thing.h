#ifndef GAMEFRAME_THING_H
#define GAMEFRAME_THING_H

#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

class Thing {
protected:
    // Physics stuff
    btCollisionShape *collisionShape;
    btRigidBody *rigidBody;
    btTransform transform;
    // Rendering stuff
    VertexArray *vao;
    Program *program;

public:
    virtual void initializeBody() = 0;

    /**
     * Update the position, etc.
     */
    virtual void updateTransform() {
        this->rigidBody->getMotionState()->getWorldTransform(transform);
    }

    void setTransform() {
        this->rigidBody->getMotionState()->setWorldTransform(transform);
    }

    glm::vec3 getPosition() {
        return (glm::vec3&)transform.getOrigin().m_floats;
    }

    virtual void setPosition(const glm::vec3& position) {
        transform.setOrigin((btVector3&)position);
        this->setTransform();
    }

    btRigidBody* getBody() {
        return this->rigidBody;
    }

    ~Thing() {
        delete rigidBody->getMotionState();
        delete rigidBody;
        delete vao;
    }
};

#endif //GAMEFRAME_THING_H
