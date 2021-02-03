#include "Entity.h"
#include "game.h"
#define FREGAMENT	0.95
#define ACCELER		1

Image height;

Entity::Entity() {

}
Entity::~Entity() {

}

void Entity::render()
{
}

void Entity::update()
{
}

//-----------------------------------House

EntityHouse::EntityHouse(Matrix44 m, Shader* sh, Mesh* mesh, Texture* text){
	model = m;
	shader = sh;
	this->mesh = mesh;
	texture = text;	
}

EntityHouse::~EntityHouse() {


}



void EntityHouse::render() {

	//upload uniforms
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
	shader->setUniform("u_texture", texture);
	shader->setUniform("u_model", model);
	shader->setUniform("camera_pos", Camera::last_enabled->eye);

	shader->setUniform("max_fog", MAX_FOG);
	shader->setUniform("min_fog", MIN_FOG);

	mesh->render(GL_TRIANGLES);
		

}

void EntityHouse::update() {

}

EntityArrow::EntityArrow(Matrix44 m, Shader* sh, Mesh* mesh, Texture* text, float time, Vector2 yaw) {
	model = m;
	this->coord0 = m.getTranslation();
	pos = coord0;
	this->yaw = yaw;
	this->last_v = Vector3(0, 0, 0);
	shader = sh;
	this->mesh = mesh;
	texture = text;
	t0 = time;
}

EntityArrow::~EntityArrow() {


}



void EntityArrow::render() {


	if (shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		shader->setUniform("u_texture", Texture::Get("data/trees/trunk.tga"));



		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
		shader->setUniform("u_model", model);
		shader->setUniform("u_time", Game::instance->time);
		shader->setUniform("camera_pos", Camera::last_enabled->eye);

		shader->setUniform("max_fog", MAX_FOG);
		shader->setUniform("min_fog", MIN_FOG);

		this->mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();

	}

}

void EntityArrow::update(float time) {
	float dt = (time - this->t0);
	last_v = this->v*70 * dt;
	Vector3 dx = last_v - Vector3(0,20, 0)*dt*dt;
	pos = this->coord0 + dx;
	
	//printf("\n yaw.y %f", yaw.y);

	Vector4 y_terrain = height.getPixelInterpolatedHigh(pos.z / MAP_SIZE * 512, pos.x / MAP_SIZE * 512);
	float min_height = y_terrain.x / 255.0 * MAX_HEIGHT + OFFSET_H;

	if (pos.y > min_height) {
		//printf("\n %f %f %f", last_v.x, last_v.y, last_v.z);
		//printf("\n moving %f %f %f", aux.x, aux.y, aux.z);
		//this->model.translate(aux.x, aux.y, aux.z);
		Matrix44 m;
		m.translate(pos.x, pos.y, pos.z);
		m.scale(0.5, 0.5, 0.5);
		
		m.rotate(yaw.x, Vector3(0, 1, 0));
		m.rotate(yaw.y, Vector3(0, 0, 1));
		this->model = m;
		Vector3 aux = this->v;
		float yy = yaw.y - aux.normalize().y*dt / 20.0;
		if(yy > -1.0)
			yaw.y = yy;
	}
}

//-------------------------------------------Tree
EntityTree::EntityTree(Matrix44 m, Shader* sh, Mesh* mesh, Texture* text) {
	model = m;
	shader = sh;
	this->mesh = mesh;
	texture = text;
}

EntityTree::~EntityTree() {


}

void EntityTree::render() {
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Game::instance->time);
	shader->setUniform("camera_pos", Camera::last_enabled->eye);

	shader->setUniform("max_fog", MAX_FOG);
	shader->setUniform("min_fog", MIN_FOG);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	float distance = Camera::last_enabled->eye.distance(Vector3(model.getTranslation().x, model.getTranslation().y, model.getTranslation().z));
	if (abs(distance) < 900) {
		shader->setUniform("u_texture", texture);
		mesh->render(GL_TRIANGLES);

	}
	else {
		
		Mesh* mesh2 = Mesh::Get("data/trees/lod_tree.obj");
		shader->setUniform("u_texture", Texture::Get("data/trees/lod_fir.tga"));
		mesh2->render(GL_TRIANGLES);
		
	}

	glDisable(GL_BLEND);
	
}

void EntityTree::update() {

}


//----------------------------------------Character

Character* Character::active_character = NULL;

Character::Character(Vector3 position, Shader* sh, Mesh* mesh, Texture* text) {
	Matrix44 m; m.translate(position.x, position.y, position.z); m.scale(0.5, 0.5, 0.5);
	model = m;
	shader = sh;
	this->mesh = mesh;
	texture = text;
	yaw = 0;
	v = Vector2(0, 0);
	this->position = position;
	anim = Animation::Get("data/walking.skanim");

	height.loadPNG(HEIGHT_FILE);
	time = 0;
	acc = 0;
	anim_mode = 0;
}

Character::~Character() {
}

void Character::render() {

	Animation *idle_apuntar = Animation::Get("data/pj/idle_apuntar.skanim");
	Animation *desenfundar = Animation::Get("data/pj/apuntar_desenfundar.skanim");
	Animation *disparar = Animation::Get("data/pj/disparar.skanim");
	Animation *apuntar = Animation::Get("data/pj/apuntar.skanim");
	Animation *idle = Animation::Get("data/pj/idle.skanim");
	Animation *correr = Animation::Get("data/pj/correr2.skanim");
	Animation *andar = Animation::Get("data/pj/andar.skanim");



	if (this->anim_mode == 1) {
		if (time - last_time < desenfundar->duration) {
			desenfundar->assignTime(this->time - last_time);
			this->anim = desenfundar;
		}
		else {
			apuntar->assignTime(this->time - last_time - desenfundar->duration);
			this->anim = apuntar;
		}
	}
	else if (this->anim_mode == 2) {
		if (time - last_time > disparar->duration) {
			anim_mode = 0;
		}
		else {
			disparar->assignTime(time - last_time);
			anim = disparar;
		}
	}

	if (this->anim_mode == 0) {

		// IDLE / ANDAR / CORRER
		float v_max = (ACC) / (1 - FREGAMENT);
		float v = this->v.y / 0.95;

		idle->assignTime((this->time));
		float timeB = this->time / andar->num_keyframes*andar->samples_per_second * correr->num_keyframes / correr->samples_per_second;
		andar->assignTime((this->time));
		correr->assignTime(timeB);


		
		blendSkeleton(&andar->skeleton, &correr->skeleton, clamp((abs(v) - 4.8) / 7.76, 0, 1), &andar->skeleton);
		blendSkeleton(&idle->skeleton, &andar->skeleton, clamp(abs(v) / 4.8, 0, 1), &idle->skeleton);
		anim = idle;

		
	}
	
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Game::instance->time);
	shader->setUniform("camera_pos", Camera::last_enabled->eye);
	shader->setUniform("max_fog", MAX_FOG);
	shader->setUniform("min_fog", MIN_FOG);
	shader->setUniform("u_texture", texture);


	mesh->renderAnimated(GL_TRIANGLES, &this->anim->skeleton);

}

void Character::update(float elapsed_time) {
	last_position = position;
	position = position + model.rotateVector(Vector3(0, 0, 1)) * v.y *elapsed_time;
	position = position + model.rotateVector(Vector3(1, 0, 0)) * v.x *elapsed_time;
	
	//v = v * 0.95;
	v = v - (v * elapsed_time) * 2.3;
	
	//v = v - (10 * elapsed_time);

	Vector4 h = height.getPixelInterpolatedHigh(this->position.z / MAP_SIZE * 512, this->position.x / MAP_SIZE * 512);
	this->position.y = h.x / 255.0 * MAX_HEIGHT + OFFSET_H;

	Matrix44 m;
	m.translate(position.x, position.y, position.z);
	m.scale(6, 6, 6);
	m.rotate(yaw*DEG2RAD, Vector3(0, 1, 0));

	this->model = m;

}
void Character::move(Vector3 move) {
	//position = position + model.rotateVector(move);
}

int aux = 0;
clock_t t2;
Animal::Animal(Vector3 position, Shader* sh, Mesh* mesh, Texture* text) {
	Matrix44 m; m.translate(position.x, position.y, position.z); m.scale(0.5, 0.5, 0.5);
	model = m;
	shader = sh;
	this->mesh = mesh;
	texture = text;
	yaw = 0;
	v = 0;
	this->position = position;
	anim = Animation::Get("data/walking.skanim");   //ficar l'animació estàtica de l'animal
	time = 0;
	acc = 0;
	behaviour = NORMAL;
	//blood.createSubdividedPlane(10);
	//steps.createSubdividedPlane(10,1,false);

	//steps.createSubdividedPlane(10,516,false);
}
Animal::~Animal() {}

void Animal::render() {
	Animation *a = Animation::Get("data/pj/idle.skanim");
	Animation *c = Animation::Get("data/pj/correr.skanim");
	Animation *b = Animation::Get("data/pj/andar.skanim");
	a->assignTime(this->time);

	float v_max = (0.05) / (1 - FREGAMENT);
	float v = this->v / 0.99;

	if (v == 0) {
		b->assignTime((this->time / b->duration));
		//a->skeleton.blend(&b->skeleton, clamp(v, 0, 1));
		this->anim = a;
	}
	else if (v>0) {
		b->assignTime(this->time / b->duration);
		c->assignTime((((this->time / b->duration)) / b->duration)*c->duration);

		//b->skeleton.blend(&c->skeleton, clamp((v - 1) / 0.6, 0, 1));
		this->anim = b;
	}


	shader->enable();

	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Game::instance->time);
	shader->setUniform("u_texture", texture);
	mesh->renderAnimated(GL_TRIANGLES, &anim->skeleton);


	shader->disable();
}
int state = 0;




void Animal::update(Character *c, std::vector<Matrix44> t, std::vector<EntityHouse> h, float dt, clock_t t1) {
	last_position = position;
	position = position + model.rotateVector(Vector3(0, 0, 1))*v;
	//elapsed time


	//Vector4 h = height.getPixelInterpolatedHigh(this->position.z / MAP_SIZE * 512, this->position.x / MAP_SIZE * 512);
	//this->position.y = h.x / 255.0 * MAX_HEIGHT + OFFSET_H;
	///////////////


	Vector4 ht = height.getPixelInterpolatedHigh(this->position.z / MAP_SIZE * 512, this->position.x / MAP_SIZE * 512);
	this->position.y = ht.x / 255.0 * MAX_HEIGHT + ANIM_OFFSET;
	//this->position.y = 65;
	//v = v * 0.95;
	Matrix44 m;
	m.translate(position.x, position.y, position.z);
	m.rotate(yaw*DEG2RAD, Vector3(0, 1, 0));
	m.scale(0.5, 0.5, 0.5);
	//renderPista(steps);
	this->model = m;
	if (behaviour == NORMAL) {
		
		//v = 0;
		//printf("1\n");
		if (checkView(c) == true) {
			behaviour = RUNNING;
		}
		checkSound(c, 100, 30);
		wandering(t1);
	}
	else if (behaviour == NERVOUS) {
		if (aux == 0) {
			t2 = clock() - t1;
			aux++;
		}
		//printf("2\n");
		checkSound(c, 0, 80);
		next_dir = calculate_angle(c);
		this->model.rotate(-next_dir * DEG2RAD, Vector3(0, 1, 0));
		yaw = -next_dir;
		long time = SDL_GetTicks();
		double sec_time = (time - t2) * 0.001;
		double countdown = 15 - sec_time;
		if (countdown < 0) { behaviour = NORMAL; aux = 0; state = 1; }
	}
	else if (behaviour == RUNNING) {
		if (aux == 0) {
			t2 = clock() - t1;
			aux++;
		}
		//printf("3\n");
		run_away(t, h, dt);
		long time = SDL_GetTicks();
		double sec_time = (time - t2) * 0.001;
		double countdown = 10 - sec_time;
		if (countdown < 0) { behaviour = NORMAL; aux = 0; }
	}
	else if (behaviour == DEAD) {

	}
}

bool Animal::checkView(Character *c) {
	Vector3 coll_point;
	Vector3 coll_normal;
	float max_ray_dist = 200;
	for (int i = yaw - 50; i < yaw + 50; i++) {
		Matrix44 m = this->model;
		m.rotate(i*DEG2RAD, Vector3(0, 1, 0));
		Vector3 ray_direction = m.rotateVector(Vector3(0, 0, 1));
		if (this->mesh->testRayCollision(c->model, this->position, ray_direction, coll_point, coll_normal, max_ray_dist, false) == true) {
			//printf("true\n");
			return true;
		}
	}
	//printf("false\n");
	return false;
}
void Animal::checkSound(Character *c, int lr, int sr) {
	float large_radius = lr * c->v.length();
	float short_radius = sr * c->v.length();
	float distance = sqrt(pow(this->position.x - c->position.x, 2) + pow(this->position.y - c->position.y, 2) + pow(this->position.z - c->position.z, 2));
	if (distance < large_radius) {
		this->behaviour = NERVOUS;
	}
	else if (distance < short_radius) {
		this->behaviour = RUNNING;
	}

}

void Animal::run_away(std::vector<Matrix44> t, std::vector<EntityHouse> h, float dt) {
	//this->acc = 0.08;
	this->v = 1.5;
	
	dodge_obstacle(t, h, dt);

}
float Animal::calculate_angle(Character *c) {
	Vector3 v1 = c->position - this->position;
	Vector3 v2 = this->model.rotateVector(Vector3(0, 0, 1));
	float dot_product = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;

	float angle = acos(dot_product / (v1.length() * v2.length()));
	float aux_yaw = int(angle*RAD2DEG) % 360;
	//printf("%d\n", aux_yaw);
	return aux_yaw;
}

void Animal::dodge_obstacle(std::vector<Matrix44> t, std::vector<EntityHouse> h, float dt) {
	Vector3 coll_point;
	Vector3 coll_normal;
	for (int i = 0; i <t.size(); i++) {
		Mesh *m = Mesh::Get("data/trees/lod_tree.obj");
		if (position.distance(t[i].getTranslation()) < 8) {
			//if (m->testSphereCollision(arbres[i], c->position, 0.5, collision_point, collision_normal) == true) {
			Vector3 push_away = normalize(t[i].getTranslation() - position) * dt;
			position = last_position - 2*push_away;
			int dec = rand() % 2;
			if (dec == 0) {
				model.rotate(35 * DEG2RAD, Vector3(0, 1, 0));
				yaw += 35;
			}
			else {
				model.rotate(-35 * DEG2RAD, Vector3(0, 1, 0));
				yaw -= 35;
			}
			//c->position.y = 0;

		}
		else {
			continue;
		}
	}
	for (std::vector<EntityHouse>::iterator it = h.begin(); it != h.end(); it++) {
		if (it->mesh->testSphereCollision(it->model, position, 4, coll_point, coll_normal) == true) {
			Vector3 push_away = normalize(coll_point - position) * dt;
			position = last_position - 3 * push_away;
			int dec = rand() % 2;
			if (dec == 0) {
				model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
				yaw += 90 ;
			}
			else {
				model.rotate(-90 * DEG2RAD, Vector3(0, 1, 0));
			yaw -= 90;
			}
			
		}
		else {
			continue;
		}
	}
}
int temp = 1, new_angle;
void Animal::wandering(clock_t t1) {
	if (aux == 0) {
		t2 = clock() - t1;
		aux++;
	}

	if (state == 1){
		//printf("1\n");
		v = 0.4;
		//model.rotate(yaw * DEG2RAD, Vector3(0, 1, 0));
		long time = SDL_GetTicks();
		double sec_time = (time - t2) * 0.001;
		double countdown = temp - sec_time;
		if (countdown < 0) { aux = 0; state = 0; }
	}
	else {
		//printf("0\n");
		v = 0;
		long time = SDL_GetTicks();
		double sec_time = (time - t2) * 0.001;
		double countdown = temp - sec_time;
		//printf("--%.f--\n", countdown);
		if (countdown < 0) { aux = 0; state = 1; temp = rand()%5+3, new_angle = rand() % 360;
		yaw += new_angle;
		}
	}

	
}

void Animal::renderPista(Mesh m) {
	Shader* current_shader = Shader::Get("data/shaders/floorPista.vs", "data/shaders/floor.fs");
	Matrix44 m2;
	m2.setIdentity();
	m2.translate(520, 0, 500);

	if (current_shader) {
		current_shader->enable();
		current_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		current_shader->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
		current_shader->setUniform("colormap", Texture::Get("data/steps.tga"));
		current_shader->setUniform("max_height", MAX_HEIGHT);

		current_shader->setUniform("u_texture_height", Texture::Get(HEIGHT_FILE));
		current_shader->setUniform("u_model", m2);
		current_shader->setUniform("detail", Texture::Get("data/Detail.tga"));
		current_shader->setUniform("camera_pos", Camera::last_enabled->eye);

		current_shader->setUniform("max_fog", MAX_FOG);
		current_shader->setUniform("min_fog", MIN_FOG);

		m.render(GL_TRIANGLES);
		current_shader->disable();
	}


}







//---------------------------------- PARTICLES
Particle::Particle(Vector3 pos, float time) {
	for (int i = 0; i < 20; i++) {
		size[i] = Vector2(100 + rand() % 200 / 100.0, 2 + rand() % 200 / 100.0);
		this->pos[i] = pos + Vector3(rand()%500 / 100.0, rand() % 500 / 100.0, rand() % 500 / 100.0);
		this->speed[i] = 2 + rand() % 100 / 100.0;
		this->brightness[i] = 200 + rand() % 55;
		this->dur[i] = 1 + rand() % 300 / 100.0;
	}
	this->t0 = time;
	
}

void Particle::render(float yaw) {

	Shader* sh = Shader::Get("data/shaders/particle.vs", "data/shaders/particle.fs");
	sh->enable();
	glDisable(GL_CULL_FACE);

	Mesh* mesh = Mesh::Get("data/box.ASE");
	Texture* texture = Texture::Get("data/trees/trunk.tga");
	Shader::current->setUniform("u_color", Vector4(1, 1, 1, 1));
	Shader::current->setUniform("u_viewprojection", Camera::last_enabled->viewprojection_matrix);
	Shader::current->setUniform("u_time", Game::instance->time);
	Shader::current->setUniform("camera_pos", Camera::last_enabled->eye);

	Shader::current->setUniform("max_fog", MAX_FOG);
	Shader::current->setUniform("min_fog", MIN_FOG);


	Shader::current->setUniform("u_texture", texture);

	std::vector<Matrix44> models;
	for (int i = 0; i < 20; i++) {
		if (brightness[i] != 0) {
			Matrix44 aux;
			aux.translate(pos[i].x, pos[i].y, pos[i].z);
			aux.rotate(yaw, Vector3(0, 1, 0));
			aux.scale(0.001, 0.01, 0.01);
			models.push_back(aux);
		}
	}

	if (models.size() > 0)
		mesh->renderInstanced(GL_TRIANGLES, &models[0], models.size());

	sh->disable();
}

void Particle::update(float time, float dt) {
	for (int i = 0; i < 20; i++) {
		this->pos[i] = this->pos[i] + Vector3(0, this->speed[i], 0)*dt;

		if ( (time - this->t0) > this->dur[i]) {
			this->brightness[i] = 0; //means it wont be printed
		}
	}
}


Particle::~Particle() {

}