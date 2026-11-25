#include "PhysicsSystem.hpp"

inline void checkCudaErrors(cudaError_t result) {
    if (result != cudaSuccess) {
        throw std::runtime_error(cudaGetErrorString(result));
    }
}


void PhysicsSystem::initialize(const RigidBody *bodies, const mat4 *body_from_locals, size_t count){
    if (deviceBodies != nullptr) throw std::runtime_error("Physics already initialized");
    if(count == 0) return;

    {
        RigidBody *allocation = nullptr;
        const size_t bytes = count * sizeof(RigidBody);
        checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&allocation), bytes));

        cudaError_t result = cudaMemcpy(
            allocation, bodies, bytes, cudaMemcpyHostToDevice
        );

        if (result != cudaSuccess) {
            cudaFree(allocation);
            checkCudaErrors(result);
        }

        deviceBodies = allocation;
    }

    {
        mat4 *allocation = nullptr;
        const size_t bytes = count * sizeof(mat4);
        checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&allocation), bytes));

        cudaError_t result = cudaMemcpy(
            allocation, body_from_locals, bytes, cudaMemcpyHostToDevice
        );

        if (result != cudaSuccess) {
            cudaFree(allocation);
            checkCudaErrors(result);
        }

        device_body_from_locals = allocation;
    }
    
    bodyCount = count;
}

PhysicsSystem::~PhysicsSystem(){
    if(deviceBodies != nullptr){
        cudaFree(deviceBodies);
    }
}