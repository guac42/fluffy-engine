#ifndef GAMEFRAME_WORLD_H
#define GAMEFRAME_WORLD_H

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>

class World {
private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld *dynamicsWorld;

    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    Camera *camera;
    btRigidBody *cameraBody;

public:
    explicit World(Camera *camera): camera(camera) {
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

        // Add camera body
        {
            btCollisionShape *colShape = new btSphereShape(btScalar(0.6));
            this->collisionShapes.push_back(colShape);

            btTransform startTransform;
            startTransform.setIdentity();
            startTransform.setOrigin(btVector3(0, 1, 0));

            btScalar mass(1.f);
            btVector3 localInertia(0, 0, 0);
            colShape->calculateLocalInertia(mass, localInertia);

            //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
            btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
            this->cameraBody = new btRigidBody(rbInfo);
            this->cameraBody->setAngularFactor(btScalar(0.));
            this->cameraBody->setDamping(btScalar(.8), btScalar(0.));
            // Could disable deactivation, but might be less efficient
            //this->cameraBody->setActivationState(DISABLE_DEACTIVATION);

            this->dynamicsWorld->addRigidBody(cameraBody);
        }
    }

    void updateWorld(float delta) {
        if (this->camera->moved) {
            btVector3 force(btScalar(this->camera->Acceleration.x), btScalar(this->camera->Acceleration.y),
                            btScalar(this->camera->Acceleration.z));
            // for some reason applying a force doesn't unsleep the body?
            this->cameraBody->activate();
            this->cameraBody->applyCentralForce(force);
        }
        int steps = this->dynamicsWorld->stepSimulation(delta);
        btTransform transform;
        this->cameraBody->getMotionState()->getWorldTransform(transform);
        printf("%d - %f, %f, %f\n", steps, float(transform.getOrigin().getX()), float(transform.getOrigin().getY()), float(transform.getOrigin().getZ()));
        this->camera->processWorldUpdate(float(transform.getOrigin().getX()), float(transform.getOrigin().getY()), float(transform.getOrigin().getZ()));
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
