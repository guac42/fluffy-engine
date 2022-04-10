#ifndef GAMEFRAME_WORLD_H
#define GAMEFRAME_WORLD_H

#include <vector>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>
#include "DebugDraw.h"
#include "Thing.h"

class World {
private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    std::vector<Thing*> things;
    DebugDraw* debugDraw;

public:
    World() {
        this->collisionConfiguration = new btDefaultCollisionConfiguration();
        this->dispatcher = new btCollisionDispatcher(this->collisionConfiguration);
        this->overlappingPairCache = new btDbvtBroadphase();
        this->solver = new btSequentialImpulseConstraintSolver;

        this->dynamicsWorld = new btDiscreteDynamicsWorld(this->dispatcher, this->overlappingPairCache, this->solver, this->collisionConfiguration);
        this->dynamicsWorld->setGravity(btVector3(0, -10, 0));

#ifdef DEBUG
        this->debugDraw = new DebugDraw();
        this->dynamicsWorld->setDebugDrawer(this->debugDraw);
#endif

        // Add ground shape
        {
            btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0., 1., 0.), btScalar(0.));
            btTransform groundTransform;
            groundTransform.setIdentity();

            //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
            btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape);
            btRigidBody* body = new btRigidBody(rbInfo);

            //add the body to the dynamics world
            this->dynamicsWorld->addRigidBody(body);
        }

        btCollisionShape* boxShape = new btBoxShape(btVector3(.5f, .5f, .5f));
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(3.f, .5f, 3.f));

        btRigidBody::btRigidBodyConstructionInfo info(0.f, nullptr, boxShape);
        btRigidBody* body = new btRigidBody(info);
        this->dynamicsWorld->addRigidBody(body);
    }

    void addThing(Thing *thing) {
        if (thing->getBody() == nullptr) {
            printf("ERROR: Thing has null rigid body\n");
            return;
        }
        this->dynamicsWorld->addRigidBody(thing->getBody());
        this->things.push_back(thing);
    }

    void updateWorld(float delta) {
        this->dynamicsWorld->stepSimulation(delta); // Physics world step
        this->dynamicsWorld->debugDrawWorld();

        for (const auto &thing : this->things)
            thing->updateTransform();

    }

#ifdef DEBUG
    void renderDebug(Camera* camera) {
        this->debugDraw->render(camera);
    }
#endif

    btDynamicsWorld* getWorld() {
        return this->dynamicsWorld;
    }

    virtual ~World() {
        // remove bodies from world and delete them
        for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);

            if (body && body->getMotionState())
                delete body->getMotionState();

            dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }

        delete dynamicsWorld;
        delete solver;
        delete overlappingPairCache;
        delete dispatcher;
        delete collisionConfiguration;
    }
};

#endif //GAMEFRAME_WORLD_H
