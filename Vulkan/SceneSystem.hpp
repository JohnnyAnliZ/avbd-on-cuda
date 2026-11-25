#pragma once
#include "RTG.hpp"
#include "MaterialSystem.hpp"
#include "S72.hpp"
#include "Light.hpp"
#include "Camera.hpp"
#include "Transform.hpp"
#include "Shadow2DPipeline.hpp"
#include "PosNorTanTexVertex.hpp"
#include "LinesPipeline.hpp"
#include "GeometryGen.hpp"
#include "mat4.hpp"
#include "PhysicsSystem.hpp"

#include <array>
#include <deque>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

struct SceneSystem {
	SceneSystem(RTG &rtg, S72 &scene72, MaterialSystem &material_system);
	~SceneSystem();

	RTG &rtg;
	S72 &scene72;
	MaterialSystem &material_system;

	struct Item {
		S72::Node *node;
		mat4 parentWorld;
	};

	struct ObjectVertices {
		uint32_t first = 0;
		uint32_t count = 0;
	};
	ObjectVertices fruit_vertices;
	Helpers::AllocatedBuffer object_vertices;

	struct Mesh {
		ObjectVertices verts;
		AABB bbox;
	};
	std::map<std::string, Mesh> meshes;

	std::unordered_map<std::string, BasicCamera> loaded_cameras;
	std::unordered_map<std::string, BasicCamera>::iterator current_camera;


	//------------CPU objects-------------
	struct LambertianObjectInstance {
		ObjectVertices vertices;
		Transform transform;
		uint32_t texture = 0;
	};
	struct EnvMirrorObjectInstance {
		ObjectVertices vertices;
		Transform transform;
		uint32_t texture = 0;
		int is_env = 1;
	};
	struct PbrObjectInstance {
		ObjectVertices vertices;
		Transform transform;
		uint32_t texture = 0;
	};

	std::vector<LambertianObjectInstance> lambertian_object_instances;
	std::vector<EnvMirrorObjectInstance> env_mirror_object_instances;
	std::vector<PbrObjectInstance> pbr_object_instances;


	//------------GPU objects-------------
	PhysicsSystem physics_system;

	struct BatchKey {
		PhysicsPipeline pipeline;
		std::string material;
		std::string mesh;

		bool operator<(const BatchKey& other) const {
			return std::tie(pipeline, material, mesh)
				< std::tie(other.pipeline, other.material, other.mesh);
		}
	};
	struct DrawBatch {
		uint32_t pipelineId;
		uint32_t materialId;
		uint32_t firstVertex;//vertex buffer shared with cpu objects
		uint32_t vertexCount;
		uint32_t firstInstance;
		uint32_t instanceCount;
	};

	std::vector<DrawBatch> draw_batches;
	
	std::vector<uint32_t> instanceBodyIds;//index into the Transform, [firstInstance, firstInstance+instanceCount) is the transforms of the batch

	float time_elapsed = 0.0f;

	//------lights and shadow stuff-------
	struct point { uint32_t x, y; };
	std::vector<Light> lights;
	std::vector<uint32_t> light_shadow_map_sizes;
	bool default_world_lights = true;

	VkPipelineLayout shadow_pipe_layout = VK_NULL_HANDLE;
	uint64_t total_shadow_map_size = 0;
	uint32_t atlas_size = 0;
	bool shadows_on = true;
	bool shadow_dump = false;
	void draw_all_objects(VkCommandBuffer const &cmd, mat4 const &LIGHTS_CLIP_FROM_WORLD, vec4 const &_shadow_atlas);

	//----Camera and view state----
	CameraMode camera_mode      = CameraMode::Scene;
	CameraMode prev_camera_mode = CameraMode::Debug;
	OrbitCamera free_camera;
	OrbitCamera debug_camera;
	mat4 CLIP_FROM_WORLD;
	vec3 EYE     = vec3(0, 0, 0);
	vec3 CAM_DIR = vec3(0, 0, -1);
	bool wind_motor_active = false;

	//----stuff for tree generation----
	//TODO: make this into a modular system 
	vec3 emplace_random_line(vec3 start, uint32_t iter);
	bool growing = true;
	std::vector<vec3> starts;
	uint32_t iters = 0;

	//----Debug lines----
	std::vector<LinesPipeline::Vertex> lines_vertices;
	void add_debug_lines_frustrum();
	void add_debug_lines_bbox(AABB &bbox, mat4 WORLD_FROM_LOCAL);
	void add_cuboid_from_corners(std::array<vec3, 8> const &box_corners, uint8_t r, uint8_t g, uint8_t b);

	void load_scene();
	void update_scene(float dt);

	bool do_cull(std::array<vec3, 8> const &frustrum_corners, std::array<vec3, 8> const &box_corners);

	void allocate_texture_atlas(point const &atlas_size, std::vector<uint32_t> const &texture_sizes);
};
