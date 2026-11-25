#pragma once
#include "mat4.hpp"
#include <cstddef>

struct RigidBody {
    vec3 position;
    quat orientation;

    vec3 linearVelocity;
    vec3 angularVelocity;

    float inverseMass;
    vec3 inverseInertia;
    vec3 halfExtents;
};

struct PhysicsInput {
    int selectedBody;
    vec3 impulse;
};


class PhysicsSystem {
public:
    PhysicsSystem() = default;
    ~PhysicsSystem();

    PhysicsSystem(const PhysicsSystem&) = delete;
    PhysicsSystem& operator=(const PhysicsSystem&) = delete;

    void initialize(const RigidBody* bodies, const mat4* body_from_locals, size_t count);

private:
	//buffer of RigidBody types, computed and uploaded once. Kept track of on GPU for 
	//physics calculations
    RigidBody* deviceBodies = nullptr;
    mat4* device_body_from_locals = nullptr;
    size_t bodyCount = 0;
};