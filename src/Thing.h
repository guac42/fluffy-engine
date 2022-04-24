#ifndef GAMEFRAME_THING_H
#define GAMEFRAME_THING_H

#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

class Thing {
protected:
    // Physics stuff
    btCollisionShape* pCollisionShape;
    btDefaultMotionState* pMotionState;
    btRigidBody* pRigidBody;
    btTransform motionTransform;

public:
    virtual void initializeBody() = 0;

    /**
     * Update the position, etc.
     */
    virtual void updateTransform() {
        this->pMotionState->getWorldTransform(motionTransform);
    }

    void setTransform() {
        this->pMotionState->setWorldTransform(motionTransform);
    }

    glm::vec3 getPosition() {
        return (glm::vec3&)motionTransform.getOrigin().m_floats;
    }

    virtual void setPosition(const glm::vec3& position) {
        motionTransform.setOrigin((btVector3&)position);
        this->setTransform();
    }

    btRigidBody* getBody() {
        return this->pRigidBody;
    }

    ~Thing() {
        delete pCollisionShape;
        delete pMotionState;
        delete pRigidBody;
    }
};

#endif //GAMEFRAME_THING_H
