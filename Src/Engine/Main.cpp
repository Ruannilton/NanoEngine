extern "C" {
#include "NanoEngine.h"
#include "..//Materials/TexturedMaterial.h"
}


 void Start() {

	mesh_loader_pre_alloc = mbyte(10);

	TexturedMaterial* t_mat = CNEW(TexturedMaterial, "Assets/Images/Cerberus_A.png");
	Material* mat = CNEW(Material,DefaultShader, TexturedMaterialbndFnc, t_mat);
	Mesh* m = mesh_LoadMesh("Assets/models/Cerberus.obj");
	
	RenderComponent* rc = RenderComponent_Create(m, mat);
	RenderComponent* rc2 = RenderComponent_Create(m, mat);
	
	RC_SetPosition(rc,  vec3{ 0.5f,0,-2.5f });;
	RC_SetPosition(rc2, vec3{ -0.5f,0,-2.5f });

	AddToRender(rc);
	AddToRender(rc2);
	
}
 
 void Update() {

	 vec3 mouse_mov = { mouse_delta_x, mouse_delta_y, 0 };
	 camera_Rotate(&currentCamera, mouse_mov);
	 if (mouse_delta_scroll_y != 0) {
		 camera_Zoom(&currentCamera, mouse_delta_scroll_y);
		
		 DEBUG_C(ANSI_MAGENTA, "Camera FOV:  %f", currentCamera.fov);
	 }
	 
	 DEBUG_C(ANSI_LIGHT_BLUE,"Mouse position:  %f %f",mouse_Xpos,mouse_Ypos);
	 DEBUG_C(ANSI_LIGHT_GREEN, "Camera looking to: %f %f %f", currentCamera.rotation[0], currentCamera.rotation[1], currentCamera.rotation[2]);
	 
	 vec3 left = { -2.5f*delta_time,0,0 };
	 vec3 right = {  2.5f * delta_time,0,0 };
	 vec3 foward = { 0,0,2.5f * delta_time };
	 vec3 backward = { 0.0f,0,-2.5f * delta_time };

	 if (Key(GLFW_KEY_A)) {
		 camera_Translate(&currentCamera, left);
	 }
	 if (Key(GLFW_KEY_D)) {
		 camera_Translate(&currentCamera, right);
	 }
	 if (Key(GLFW_KEY_W)) {
		 camera_Translate(&currentCamera, foward);
	 }
	 if (Key(GLFW_KEY_S)) {
		 camera_Translate(&currentCamera, backward);
	 }
	 
 }