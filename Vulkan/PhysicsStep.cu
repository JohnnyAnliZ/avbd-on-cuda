#include <PhysicsSystem.hpp>

struct AABB {
    vec3 lo;
    vec3 hi;
};



void AVBD_step(RigidBody *bodies, PhysicsInput input){
    //AABBs
    //	current poses -> object AABBs

    //LBVH construction
    //	objects AABBs-> LBVH

    //LBVH traversal 
    //	LBVH -> potential collision pair indices

    //Narrow-phase
    //	

    //Graph coloring
    //	contacting dynamic bodies must have different colors.
    
    //AVBD solve (N iterations)
    //	colored bodies with transforms and velocities -> new transforms and velocities

    //AVBD update
    //	apply 

}

__global__ void poses_to_AABBs(RigidBody * bodies){
    
}


