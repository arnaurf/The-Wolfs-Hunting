#include "World.h"
#include "camera.h"

World::World()
{
	plane.createSubdividedPlane(MAP_SIZE, 513, false);
}


World::~World()
{
}

void World::renderFloor() {
	Shader* current_shader = Shader::Get("data/shaders/floor.vs", "data/shaders/floor.fs");
	Matrix44 m2;
	m2.setIdentity();

	if (current_shader) {
		current_shader->enable();
		current_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		current_shader->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
		current_shader->setUniform("colormap", Texture::Get("data/colormap.tga"));
		current_shader->setUniform("max_height", MAX_HEIGHT);

		current_shader->setUniform("u_texture_height", Texture::Get(HEIGHT_FILE));
		current_shader->setUniform("u_model", m2);
		current_shader->setUniform("detail", Texture::Get("data/Detail.tga"));
		current_shader->setUniform("camera_pos", Camera::last_enabled->eye);

		current_shader->setUniform("max_fog", MAX_FOG);
		current_shader->setUniform("min_fog", MIN_FOG);

		plane.render(GL_TRIANGLES);
		current_shader->disable();
	}
}

void World::renderSky() {
	Shader* current_shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");;

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	if (current_shader) {
		Matrix44 model;
		current_shader->enable();
		model.translate(Camera::last_enabled->eye.x, Camera::last_enabled->eye.y, Camera::last_enabled->eye.z);

		current_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		current_shader->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
		current_shader->setUniform("mask", Texture::Get("data/sky.tga"));
		current_shader->setUniform("u_model", model);
		Mesh::Get("data/sphere.obj")->render(GL_TRIANGLES);
		current_shader->disable();
	}
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void World::render() {

	//set flags 
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	
	renderSky();
	renderFloor();
	/*
	int ch_pos_x = int(characters.begin()->position.x / MAP_SIZE * 10);
	int ch_pos_z = int(characters.begin()->position.z / MAP_SIZE * 10);
	for (int i = 0; i < this->animal_pos[ch_pos_x][ch_pos_z].size(); i++) {

		for (auto it = this->animal_pos[ch_pos_x][ch_pos_z].begin(); it != this->animal_pos[ch_pos_x][ch_pos_z].end(); i++) {
			
		}

	}*/
	// ---------------------- RENDER HOUSES
	Shader* tree = Shader::Get("data/shaders/trees.vs", "data/shaders/trees.fs");
	tree->enable();
	for (int i = 0; i < 49; i++) {
		for (int j = 0; j < 49; j++) {
			for (std::vector<EntityHouse>::iterator it = World::houses[i][j].begin(); it != World::houses[i][j].end(); it++) {
				if (abs(it->model.getTranslation().distance(Character::active_character->position)) < MAX_RENDER
					&& Camera::last_enabled->testBoxInFrustum(it->model.getTranslation(), Vector3(200, 25, 3)))
					it->render();
			}
		}
	}
	tree->disable();


	// ------------------------ CHARACTERS
	Shader* shader = Shader::Get("data/shaders/anim.vs", "data/shaders/texture.fs");
	shader->enable();
	for (std::vector<Character>::iterator it = World::characters.begin(); it != World::characters.end(); it++) {
		
		float dist = abs(it->position.distance(Character::active_character->position));
		if (dist< MAX_RENDER && Camera::last_enabled->testBoxInFrustum(it->model.getTranslation(), Vector3(20,25, 20)) )
			it->render();
				
	}
	shader->disable();

	for (std::vector<EntityArrow>::iterator it = World::arrows.begin(); it != World::arrows.end(); it++) {
		//printf("\n coord %f %f %f", it->model.getTranslation().x, it->model.getTranslation().y, it->model.getTranslation().z);
		it->render();
	}

	for (std::vector<Animal>::iterator it = World::animals.begin(); it != World::animals.end(); it++) {
		float dist = abs(it->position.distance(Character::active_character->position));

		if (dist< MAX_RENDER && Camera::last_enabled->testBoxInFrustum(it->model.getTranslation(), Vector3(20, 25, 20)))
			it->render();
	}


	tree->enable();
	Matrix44 mat;
	characters.begin()->anim->skeleton.updateGlobalMatrices();
	Matrix44 rot; rot.rotate(-PI / 2.0, Vector3(1, 0, 0));


	Matrix44 pos = characters.begin()->anim->skeleton.getBoneMatrix("mixamorig_LeftHandThumb3", false);


	tree->setUniform("u_color", Vector4(1, 1, 1, 1));
	tree->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
	tree->setUniform("u_model", rot*pos*characters.begin()->model);
	tree->setUniform("u_time", Game::instance->time);
	tree->setUniform("camera_pos", Camera::last_enabled->eye);
	tree->setUniform("max_fog", MAX_FOG);
	tree->setUniform("min_fog", MIN_FOG);
	tree->setUniform("u_texture", Texture::Get("data/trees/trunk.tga"));

	Mesh *bow = Mesh::Get("data/pj/Bow.obj");
	bow->render(GL_TRIANGLES);
	tree->disable();
	/*
	rot.rotate(2.5*PI / 4.0, Vector3(0, 0, 1));
	rot.scale(0.07, 0.07, 0.07);
	Mesh* arrow = Mesh::Get("data/arrow.obj");
	tree->setUniform("u_model", rot*pos*characters.begin()->model);
	tree->setUniform("u_color", Vector4(1, 1, 1, 1));
	tree->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
	tree->setUniform("camera_pos", Camera::last_enabled->eye);

	tree->setUniform("max_fog", MAX_FOG);
	tree->setUniform("min_fog", MIN_FOG);

	arrow->render(GL_TRIANGLES);
	*/

	
	
	
	// ----------------------- TREES
	Shader* tree_i = Shader::Get("data/shaders/trees_instant.vs", "data/shaders/trees.fs");
	tree_i->enable();

	renderTrees(World::arbres, "data/trees/lod_birch.tga");
	renderTrees(World::arbres_red, "data/trees/lod_poplar2.tga");
	renderTrees(World::arbres_yellow, "data/trees/lod_poplar.tga");
	tree_i->disable();

	for (int i = 0; i < particles.size(); i++) {
		Vector2 aux = Vector2(Camera::last_enabled->eye.x - particles[i].pos->x, Camera::last_enabled->eye.z - particles[i].pos->z);
		particles[i].render( atan(aux.y/aux.x));
	}

	
}

void World::renderTrees(std::vector<Matrix44> models[50][50], char* texture_name) {

	glDisable(GL_CULL_FACE);
	
	Mesh* mesh = Mesh::Get("data/trees/lod_tree.obj");
	Texture* texture = Texture::Get(texture_name);
	Shader::current->setUniform("u_color", Vector4(1, 1, 1, 1));
	Shader::current->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
	Shader::current->setUniform("u_time", Game::instance->time);
	Shader::current->setUniform("camera_pos", Camera::last_enabled->eye);

	Shader::current->setUniform("max_fog", MAX_FOG);
	Shader::current->setUniform("min_fog", MIN_FOG);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_ADD);

	//world.trees.push_back(EntityTree(m, tree, Mesh::Get("data/trees/trunk.obj"), Texture::Get("data/trees/trunk.tga")));
		//world.trees.push_back(EntityTree(m, tree, Mesh::Get("data/trees/leaves.obj"), Texture::Get("data/trees/leaves_poplar.tga")));
	Shader::current->setUniform("u_texture", texture);
	std::vector<Matrix44> a;
		for (int i = 0; i < 50; i++) {
			for (int j = 0; j < 50; j++) {
				for (int k = 0; k < models[i][j].size(); k++) {
				if (abs(models[i][j].at(k).getTranslation().distance(Character::active_character->position)) < MAX_RENDER
					&& Camera::last_enabled->testBoxInFrustum(models[i][j].at(k).getTranslation(), Vector3(10, 40, 10))) {

					a.push_back(models[i][j].at(k));

				}
			}
		}

	}

	//printf("\n %i", a.size());
	if (a.size() > 0)
		mesh->renderInstanced(GL_TRIANGLES, &a[0], a.size());

	/*float distance = Camera::last_enabled->eye.distance(Vector3(model.getTranslation().x, model.getTranslation().y, model.getTranslation().z));
	if (abs(distance) < 900) {
		tree->setUniform("u_texture", texture);
		mesh->render(GL_TRIANGLES);

	}
	else {
		Mesh* mesh2 = Mesh::Get("data/trees/lod_tree.obj");
		tree->setUniform("u_texture", Texture::Get("data/trees/lod_fir.tga"));
		mesh2->render(GL_TRIANGLES);
	}*/
	glDisable(GL_BLEND);
}
bool World::checkCollision(Character * c, float dt) {
	Vector3 character_center = c->position + Vector3(0, 4, 0);

	Vector3 collision_point;
	Vector3 collision_normal;

	for (std::vector<EntityHouse>::iterator it = World::houses[int(character_center.x/ MAP_SIZE * 49)][int(character_center.z/ MAP_SIZE * 49)].begin(); it != World::houses[int(character_center.x/ MAP_SIZE * 49)][int(character_center.z/ MAP_SIZE * 49) ].end(); it++) {
		if (it->mesh->testSphereCollision(it->model, character_center, 2, collision_point, collision_normal) == true) {
			Vector3 push_away = normalize(collision_point - character_center) * dt;
			c->position = c->last_position - push_away;
			return true;
			//c->position.y = 0;
		}
		else {
			continue;
		}
	}
	for (int i = 0; i < World::arbres[int(character_center.x/ MAP_SIZE * 49)][int(character_center.z/ MAP_SIZE * 49)].size(); i++) {
		Mesh *m = Mesh::Get("data/trees/lod_tree.obj");
		if ( c->position.distance(arbres[int(character_center.x/ MAP_SIZE * 49)][int(character_center.z/ MAP_SIZE * 49)][i].getTranslation()) < 5){
		//if (m->testSphereCollision(arbres[i], c->position, 0.5, collision_point, collision_normal) == true) {
			Vector3 push_away = normalize(arbres[int(character_center.x/ MAP_SIZE * 49)][int(character_center.z/ MAP_SIZE * 49)][i].getTranslation() - character_center) * dt;
			c->position = c->last_position - 1.5*push_away;
			return true;
			//c->position.y = 0;

		}
		else {
			continue;
		}
	}
	return false;

}


