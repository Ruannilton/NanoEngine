#ifndef COLOR_MATERIAL
#define COLOR_MATERIAL

#include "..//Engine/Graphic/Material.h"
#include "..//Engine/Graphic/Texture.h"
#include "../Engine/Graphic/Color.h"
#include "../Engine/Graphic/NanoRender.h"



typedef struct {
	Color color;
}ColorMaterial;

Material_CreateBindFnc(ColorMaterial) {
	
	vec3 ambient = { 0.05375f,0.05f,0.06625f };
	vec3 diffuse = { 0.18275f,0.17f,0.63f };
	vec3 specular = { 0.332741f,0.328634f,0.346435f };

	Shader_SetVec3(shader, "material.Ambient", ambient);
	Shader_SetVec3(shader, "material.Diffuse", diffuse);
	Shader_SetVec3(shader, "material.Specular", specular);
	Shader_SetFloat(shader, "material.Shininess", 0.3f * 128);

	Shader_SetVec3(shader, "CameraPos", current_camera.position);
	Shader_SetVec4(shader, "color", ((ColorMaterial*)data)->color.arr);

	

}

inline Material* ColorMaterial_CTR(ColorMaterial* self, Shader shader, Color c) {
	self->color = c;
	Material* mat = CNEW(Material, shader, ColorMaterialbndFnc, self);
	return mat;
}

#endif