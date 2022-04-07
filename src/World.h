#ifndef GAMEFRAME_WORLD_H
#define GAMEFRAME_WORLD_H

#include <vector>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>
#include "Player.h"
#include "Client.h"

class World {
private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld *dynamicsWorld;

    btAlignedObjectArray<btCollisionShape*> collisionShapes;
    std::vector<Player*> players;

public:
    World() {
        this->collisionConfiguration = new btDefaultCollisionConfiguration();
        this->dispatcher = new btCollisionDispatcher(this->collisionConfiguration);
        this->overlappingPairCache = new btDbvtBroadphase();
        this->solver = new btSequentialImpulseConstraintSolver;

        this->dynamicsWorld = new btDiscreteDynamicsWorld(this->dispatcher, this->overlappingPairCache, this->solver, this->collisionConfiguration);
        this->dynamicsWorld->setGravity(btVector3(0, -10, 0));

        // Add ground shape
        {
            btCollisionShape *groundShape = new btStaticPlaneShape(btVector3(0., 1., 0.), btScalar(0.));
            this->collisionShapes.push_back(groundShape);

            btTransform groundTransform;
            groundTransform.setIdentity();

            //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
            btDefaultMotionState *myMotionState = new btDefaultMotionState(groundTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape);
            btRigidBody *body = new btRigidBody(rbInfo);

            //add the body to the dynamics world
            this->dynamicsWorld->addRigidBody(body);
        }

    }

    void addPlayer(Player *player) {
        this->dynamicsWorld->addRigidBody(player->getBody());
        players.push_back(player);
    }

    void updateWorld(float delta) {
        this->dynamicsWorld->stepSimulation(delta); // Physics world step

        for (const auto &player : this->players)
            player->update();

    }

    virtual ~World() {
        printf("Delete World\n");
        int i;
        // remove bodies from world and delete them
        for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body && body->getMotionState())
            {
                delete body->getMotionState();
            }
            dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }

        // delete collision shapes
        for (i = 0; i < collisionShapes.size(); i++)
        {
            btCollisionShape* shape = collisionShapes[i];
            collisionShapes[i] = 0;
            delete shape;
        }

        delete dynamicsWorld;
        delete solver;
        delete overlappingPairCache;
        delete dispatcher;
        delete collisionConfiguration;
    }
};

#endif //GAMEFRAME_WORLD_H
