#ifndef ENTITY_H
#define ENTITY_H

#include "utils.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "camera.h"
#include "stage.h"
#include "game.h"
#include "animation.h"
#include <stdlib.h>
#include <math.h> 
#include <time.h>

class Entity
{
public:
	Matrix44 model;
	Shader* shader;
	Mesh* mesh;
	Texture* texture;

	Entity();
	~Entity();
	virtual void render();
	virtual void update();

};

class EntityHouse : public Entity
{
public:
	EntityHouse(Matrix44 m, Shader* sh, Mesh* mesh, Texture* text);
	~EntityHouse();
	void render();
	void update();
};

class EntityArrow : public Entity
{
public:
	Vector2 yaw;
	Vector3 v;
	Vector3 last_v;
	Vector3 pos;
	float t0;
	Vector3 coord0;

	EntityArrow(Matrix44 m, Shader* sh, Mesh* mesh, Texture* text, float time, Vector2 yaw);
	~EntityArrow();
	void render();
	void update(float time);

};

class EntityTree : public Entity
{
public:
	Mesh* fulles;
	EntityTree(Matrix44 m, Shader* sh, Mesh* mesh, Texture* text);
	~EntityTree();
	void render();
	void update();
};

class Character : public Entity
{
public:
Character(Vector3 position, Shader* sh, Mesh* mesh, Texture* text);
	~Character();
	static Character* active_character;

	Animation* anim;
	Vector3 position;
	Vector3 last_position;
	
	float yaw;
	Vector2 v;
	float acc;

	int anim_mode;
	float time;
	float last_time;

	void render();
	void update(float elapsed_time);
	void move(Vector3 move);
};

class Animal : public Entity
{
public:
	Animal(Vector3 position, Shader* sh, Mesh* mesh, Texture* text);
	Mesh blood;
	Mesh steps;
	~Animal();
	Animation* anim;
	Vector3 position;
	Vector3 last_position;
	int vida;
	float time;
	float yaw;
	float v;
	float acc;
	float next_dir;
	enum behaviours {
		NORMAL,
		NERVOUS,
		RUNNING,
		DEAD
	};
	behaviours behaviour;
	void render();
	void update(Character *c, std::vector<Matrix44> t, std::vector<EntityHouse> h, float dt, clock_t t1);
	bool checkView(Character *c);
	void checkSound(Character *c, int lr, int sr);
	bool checkArrow(std::vector<EntityArrow> arrows);

	void run_away(std::vector<Matrix44> t, std::vector<EntityHouse> h, float dt);
	float calculate_angle(Character *c);
	void dodge_obstacle(std::vector<Matrix44> t, std::vector<EntityHouse> h, float dt);
	void wandering(clock_t t1);

	void renderPista(Mesh m);

};
#endif

class Particle : public Entity
{
public:
	float brightness[20];
	Vector2 size[20];
	Vector3 pos[20];
	float speed[20];
	float dur[20];
	float t0;

	Particle(Vector3 pos, float time);
	~Particle();
	void render(float yaw);
	void update(float time, float dt);


};