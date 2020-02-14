#ifndef NANO_INSTANCE_TABLE
#define NANO_INSTANCE_TABLE


#include "../Utils.h"
#include "../Graphic/Material.h"
#include "../DataStructures/dynamic_vector.h"
#include "../DataStructures/hash_map.h"
#include "../Graphic/RenderComponent.h"

typedef struct {
	mat4 transform;
	Material material;
}Instance;

typedef struct {
	uint mesh_id;
	dynamic_vector instances;
}InstanceList;

InstanceList* instance_list_create(InstanceList* il, uint vao, uint init_size);

#endif