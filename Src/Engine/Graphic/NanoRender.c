#include "NanoRender.h"

#define MAT_SIZE (2 * sizeof(mat4))
#define LIGHT_SIZE (sizeof(vec4) + 5*(sizeof(vec4)) + 256*128)

void Renderer_Init() {
	
	glViewport(0, 0, NanoApplication->windowWidht, NanoApplication->windowHeight);
	glGenBuffers(1, &matrix_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer);
	glBufferData(GL_UNIFORM_BUFFER, MAT_SIZE, 0, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, SHADER_UNIFORM_MATRIX_LOC, matrix_buffer, 0, MAT_SIZE);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &lights_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, lights_buffer);
	glBufferData(GL_UNIFORM_BUFFER, LIGHT_SIZE, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, SHADER_UNIFORM_LIGHT_LOC, lights_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer_SetupProjection() {
	if (current_camera != NULL) {
		glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), current_camera->projection);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

void Renderer_SetupLighting() {
	
	glBindBuffer(GL_UNIFORM_BUFFER, lights_buffer);
	float counts[2] = {current_scene->point_lights.count,current_scene->spot_lights.count};
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec2), counts);                            
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(vec3), current_scene->sun.Ambient.arr);   
	glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(vec3), current_scene->sun.Diffuse.arr);   
	glBufferSubData(GL_UNIFORM_BUFFER, 48, sizeof(vec3), current_scene->sun.Specular.arr);  
	glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(vec3), current_scene->sun.Direction.arr); 
	glBufferSubData(GL_UNIFORM_BUFFER, 80, sizeof(vec3), current_scene->sun.Color.arr);

	float attenuation[3] = {0,0,0};
	float cuttOff[2] = { 0,0 };

	size_t index = 96;
	Iterator light_iter = Iterator_Get(&current_scene->point_lights);
	for (; Iterator_Next(&light_iter); index += 128) {

		PointLight* pl = light_iter.data;
		attenuation[0] = pl->Constant;
		attenuation[1] = pl->Linear;
		attenuation[2] = pl->Quadratic;

		glBufferSubData(GL_UNIFORM_BUFFER, index, sizeof(vec3), pl->Ambient.arr);    
		glBufferSubData(GL_UNIFORM_BUFFER, index + 16, sizeof(vec3), pl->Diffuse.arr);  
		glBufferSubData(GL_UNIFORM_BUFFER, index + 32, sizeof(vec3), pl->Specular.arr); 
		glBufferSubData(GL_UNIFORM_BUFFER, index + 48, sizeof(vec3), pl->Position.arr); 
		glBufferSubData(GL_UNIFORM_BUFFER, index + 64, sizeof(vec3), attenuation);
		glBufferSubData(GL_UNIFORM_BUFFER, index + 80, sizeof(vec3), pl->Color.arr);
	}

	light_iter = Iterator_Get(&current_scene->spot_lights);
	for (; Iterator_Next(&light_iter); index += 128) {

		SpotLight* sl = light_iter.data;
		
		attenuation[0] = sl->Constant;
		attenuation[1] = sl->Linear;
		attenuation[2] = sl->Quadratic;
		cuttOff[0] = sl->cutOff;
		cuttOff[1] = sl->outerCutOff;

		glBufferSubData(GL_UNIFORM_BUFFER, index, sizeof(vec3), sl->Ambient.arr);    
		glBufferSubData(GL_UNIFORM_BUFFER, index + 16, sizeof(vec3), sl->Diffuse.arr);  
		glBufferSubData(GL_UNIFORM_BUFFER, index + 32, sizeof(vec3), sl->Specular.arr); 
		glBufferSubData(GL_UNIFORM_BUFFER, index + 48, sizeof(vec3), sl->Position.arr); 
		glBufferSubData(GL_UNIFORM_BUFFER, index + 64, sizeof(vec3), attenuation); 
		glBufferSubData(GL_UNIFORM_BUFFER, index + 80, sizeof(vec3), sl->Color.arr);
		glBufferSubData(GL_UNIFORM_BUFFER, index + 96, sizeof(vec3), sl->Direction.arr);
		glBufferSubData(GL_UNIFORM_BUFFER, index + 112, sizeof(vec2), cuttOff);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void Renderer_SetScene(Scene* scn) {
	current_scene = scn;
	current_camera = scn->camera_scene;
	Renderer_SetupProjection();
}

void Renderer_RenderScene() {
	Camera_UpdateView(current_camera);
	
	
	glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), current_camera->projection);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), current_camera->view);
	glBindBufferRange(GL_UNIFORM_BUFFER, SHADER_UNIFORM_MATRIX_LOC, matrix_buffer, 0, 2 * sizeof(mat4));
	glUseProgram(1);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	

	Renderer_SetupLighting();
	RenderData* rd = Dictionary_Get(RenderData, &(current_scene->render_data), 1);
	InstanceList* il = Dictionary_Get(InstanceList, &rd->renderer_lists, 1);
	Instance* instance = Vector_Get(Instance, &il->instances, 0);
	
	Shader_SetInt(1, "ourTexture", 0);
	Shader_SetTextureUnit(1, GL_TEXTURE0);

	Shader_SetInt(1, "material.diffuse", 1);
	Shader_SetTextureUnit(1, GL_TEXTURE1);

	Shader_SetInt(1, "material.specular", 2);
	Shader_SetTextureUnit(1, GL_TEXTURE2);

	Shader_SetFloat(1, "material.Shininess", 32.0f);

	glBindVertexArray(1);
	glUniformMatrix4fv(SHADER_MODEL_LOC, 1, GL_FALSE, (GLfloat*) instance->transform);
	glDrawElements(GL_TRIANGLES, instance->index_count, GL_UNSIGNED_INT, 0);

	/*register uint i = 0;
	Material* cMat;

	for (; i < render_list_count; i++) {
		cMat = render_list[i]->mat;

		if (cMat != NULL) {
			glUseProgram(cMat->shader_id);
			cMat->fnc(cMat->shader_id, cMat->data);
		}
		else {
			glUseProgram(DefaultShader);
		}
		
        glUniformMatrix4fv(SHADER_MODEL_LOC, 1, GL_FALSE,(GLfloat*) render_list[i]->transform);
		glBindVertexArray(render_list[i]->mesh->mesh_id);
		glDrawElements(GL_TRIANGLES, render_list[i]->mesh->index_count, GL_UNSIGNED_INT, 0);
	
	}*/
	
	glBindVertexArray(0);
}

