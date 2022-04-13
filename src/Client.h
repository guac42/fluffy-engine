#ifndef GAMEFRAME_FRSTPERSONPLAYER_H
#define GAMEFRAME_FRSTPERSONPLAYER_H

#include "gfx/Window.h"
#include "Player.h"
#include "Camera.h"
#include "Ui.h"
#include "World.h"
#include "utils/Utils.h"

class IgnoreBodyAndGhostCast :
        public btCollisionWorld::ClosestRayResultCallback {
private:
    btRigidBody* m_pBody;
    btPairCachingGhostObject* m_pGhostObject;

public:
    IgnoreBodyAndGhostCast(btRigidBody* pBody, btPairCachingGhostObject* pGhostObject)
            : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)),
              m_pBody(pBody), m_pGhostObject(pGhostObject)
    {
    }

    btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) override {
        if (rayResult.m_collisionObject == m_pBody || rayResult.m_collisionObject == m_pGhostObject)
            return 1.0f;

        return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
    }
};

// Physics adapted from:
// https://github.com/222464/EvolvedVirtualCreaturesRepo/blob/master/VirtualCreatures/Volumetric_SDL/Source/SceneObjects/Physics/DynamicCharacterController.cpp
// Was unable to get the ghost object to work :/
class Client : public Camera {
private:
    Window* window;
    World* world;

    // All of these should be const
    // Some aren't for debug
    const float radius = .3f, height = .6f,
            bottomYOffset = height * .5f + radius,
            bottomRoundedRegionYOffset = height * .5f;
    float accelerationConstant = 5.f;
    float decelerationConstant = .8f;
    float maxSpeed = 3.f;
    float jumpImpulse = 5.f;
    const float stepHeight = .2f;

    bool hittingWall = false, onGround = false, input = false;

    const int jumpRechargeTime = 200;   // Min milliseconds between jumps
    unsigned long long msSinceJump = 0; // Milliseconds since last jump

    btCollisionShape* pCollisionShape;
    btDefaultMotionState* pMotionState;
    btRigidBody* pRigidBody;
    btPairCachingGhostObject* pGhostObject;

    btVector3 manualVelocity, previousPosition;
    btTransform motionTransform;

    std::vector<btVector3> surfaceHitNormals;

    // Ui
    bool show = true;

    void parseGhostContacts() {
        btPersistentManifold** manifoldArray = world->getWorld()->getDispatcher()->getInternalManifoldPointer();
        int numManifolds = world->getWorld()->getDispatcher()->getNumManifolds();

        // Set false now, may be set true in test
        hittingWall = false;
        surfaceHitNormals.clear();

        // TODO On ground is still kinda broken (prob point.getDist)
        // For every contact in the world (horribly inefficient, but ghost doesn't work)
        for (int i = 0; i < numManifolds; ++i) {
            btPersistentManifold* pManifold = manifoldArray[i];

            // If contact doesn't include body return
            if (pManifold->getBody0() != pRigidBody && pManifold->getBody1() != pRigidBody)
                continue;

            for (int j = 0; j < pManifold->getNumContacts(); ++j) {
                const btManifoldPoint &point = pManifold->getContactPoint(j);
                // Make sure full collision in order to not over cancel velocity
                if (point.getDistance() <= 0) {
                    if (point.m_positionWorldOnB.getY() <
                        motionTransform.getOrigin().getY() - bottomRoundedRegionYOffset) {
                        onGround = true;
                    } else {
                        hittingWall = true;
                        surfaceHitNormals.push_back(point.m_normalWorldOnB);
                    }
                }
            }
        }
    }

    void updatePosition() {
        // Ray cast, ignore rigid body
        IgnoreBodyAndGhostCast rayCallBack_bottom(pRigidBody, pGhostObject);
        world->getWorld()->rayTest(pRigidBody->getWorldTransform().getOrigin(),
                                   pRigidBody->getWorldTransform().getOrigin() - btVector3(0.0f, bottomYOffset + stepHeight, 0.0f),
                                   rayCallBack_bottom);

        // Bump up if hit
        if (rayCallBack_bottom.hasHit()) {
            float previousY = pRigidBody->getWorldTransform().getOrigin().getY();
            pRigidBody->getWorldTransform().getOrigin().setY(previousY + (bottomYOffset + stepHeight) * (1.0f - rayCallBack_bottom.m_closestHitFraction));

            btVector3 vel(pRigidBody->getLinearVelocity());
            vel.setY(0.0f);
            pRigidBody->setLinearVelocity(vel);

            onGround = true;
        }

        float testOffset = 0.07f;

        // Ray cast, ignore rigid body
        IgnoreBodyAndGhostCast rayCallBack_top(pRigidBody, pGhostObject);
        world->getWorld()->rayTest(pRigidBody->getWorldTransform().getOrigin(),
                                   pRigidBody->getWorldTransform().getOrigin() + btVector3(0.0f, bottomYOffset + testOffset, 0.0f),
                                   rayCallBack_top);

        // Bump up if hit
        if (rayCallBack_top.hasHit()) {
            pRigidBody->getWorldTransform().setOrigin(previousPosition);

            btVector3 vel(pRigidBody->getLinearVelocity());
            vel.setY(0.0f);
            pRigidBody->setLinearVelocity(vel);
        }

        previousPosition = pRigidBody->getWorldTransform().getOrigin();
    }

    void updateVelocity(float delta) {
        // Adjust only xz velocity
        manualVelocity.setY(pRigidBody->getLinearVelocity().getY());
        pRigidBody->setLinearVelocity(manualVelocity);

        // Decelerate
        if (!input)
            manualVelocity *= btPow(1.f - decelerationConstant, delta); // delta-seconds

        // If not hitting wall don't run wall code
        if (!hittingWall)
            return;

        // Wall velocity cancellation
        for (auto & surfaceHitNormal : surfaceHitNormals) {
            // Cancel velocity across normal
            btVector3 velInNormalDir((manualVelocity.dot(surfaceHitNormal) / surfaceHitNormal.length2()) * surfaceHitNormal);

            // Apply correction
            manualVelocity -= velInNormalDir * 1.05f;
        }
    }

#define IS_DOWN(x) (this->window->keyboardManager.isKeyDown(x))

    void walk(float delta) {
        btVector3 forward = btVector3(Camera::front.x, 0, Camera::front.z),
                right = btVector3(Camera::right.x, 0, Camera::right.z);

        btVector3 dir =
                ((float)IS_DOWN(GLFW_KEY_W) * forward) -
                ((float)IS_DOWN(GLFW_KEY_S) * forward) +
                ((float)IS_DOWN(GLFW_KEY_A) * right) -
                ((float)IS_DOWN(GLFW_KEY_D) * right);

        // No input, so return
        input = !dir.isZero();
        if (!input) return;

        // Normalize dir and convert to velocity
        dir.normalize() *= delta * accelerationConstant;
        btVector3 velocityXZ(dir.getX() + manualVelocity.getX(), 0.f, dir.getZ() + manualVelocity.getZ());

        // Prevent from going over maximum speed
        float speedXZ = velocityXZ.length();

        // TODO: Make velocity relative to yaw

        if (speedXZ > maxSpeed)
            velocityXZ = velocityXZ / speedXZ * maxSpeed;

        manualVelocity.setX(velocityXZ.getX());
        manualVelocity.setZ(velocityXZ.getZ());
    }

    void jump() {
        if (!onGround || !IS_DOWN(GLFW_KEY_SPACE) || msSinceJump < jumpRechargeTime)
            return;

        msSinceJump = 0;
        pRigidBody->applyCentralImpulse(btVector3(0.0f, jumpImpulse, 0.0f));

        // Move upwards slightly so velocity isn't immediately canceled when it detects it as on ground next frame
        const float jumpYOffset = 0.01f;
        float previousY = pRigidBody->getWorldTransform().getOrigin().getY();
        pRigidBody->getWorldTransform().getOrigin().setY(previousY + jumpYOffset);
    }

#undef IS_DOWN

public:
    Client(Window* window, World* world, btVector3 position = btVector3(0.f, 1.f, 0.f))
            : window(window), world(world), manualVelocity(0.0f, 0.0f, 0.0f) {
        Camera::updateView((glm::vec3&)position.m_floats);

        // Create projection matrix
        Camera::resize(window->width, window->height);

        // Physics
        pCollisionShape = new btCapsuleShape(radius, height);

        btVector3 intertia;
        pCollisionShape->calculateLocalInertia(1.f, intertia);
        pMotionState = new btDefaultMotionState(btTransform(btQuaternion(1.0f, 0.0f, 0.0f, 0.0f).normalized(), position));
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1.f, pMotionState, pCollisionShape, intertia);

        // No friction, this is done manually
        rigidBodyCI.m_friction = 0.0f;
        rigidBodyCI.m_restitution = 0.0f;
        rigidBodyCI.m_linearDamping = 0.0f;

        pRigidBody = new btRigidBody(rigidBodyCI);
        pRigidBody->setAngularFactor(0.0f);
        pRigidBody->setActivationState(DISABLE_DEACTIVATION); // No sleeping (or else setLinearVelocity won't work)
        pRigidBody->setCollisionFlags(btRigidBody::CollisionFlags::CF_DISABLE_VISUALIZE_OBJECT);

        world->getWorld()->addRigidBody(pRigidBody);

        // Ghost object that is synchronized with rigid body
        pGhostObject = new btPairCachingGhostObject();
        pGhostObject->setCollisionShape(pCollisionShape);
        pGhostObject->setUserPointer(this);
        pGhostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

        // Specify filters manually, otherwise ghost doesn't collide with statics for some reason
        //world->getWorld()->addCollisionObject(pGhostObject, btBroadphaseProxy::KinematicFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    }

    void updateFrame() {
        if (!window->cursorLocked) return;   // Don't update if in Ui
        Camera::updateFrame(this->window);

        // Sync ghost with actual object
        //pGhostObject->setWorldTransform(pRigidBody->getWorldTransform());

        // Update transform
        pMotionState->getWorldTransform(motionTransform);
        onGround = false;

        walk(window->deltaTime());
        parseGhostContacts();

        updatePosition();

        jump();
        updateVelocity(window->deltaTime());

        // Update view matrix
        Camera::updateView((glm::vec3&)previousPosition.m_floats);

        // Add frame delta in milliseconds
        this->msSinceJump += window->frame_delta / 1000000ull;
    }

    void updateGui() {
        if (!Ui::isActive()) return; // Don't push ui if not active
        if (ImGui::Begin("Client", &this->show)) {
            ImGui::SliderFloat("Friction", &decelerationConstant, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Acceleration", &accelerationConstant, 0.0f, 10.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Jump Factor", &jumpImpulse, 0.0f, 10.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Max Speed", &maxSpeed, 0.0f, 10.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::End();
    }

    void resize() {
        Camera::resize(window->width, window->height);
    }
};

#endif //GAMEFRAME_FRSTPERSONPLAYER_H
