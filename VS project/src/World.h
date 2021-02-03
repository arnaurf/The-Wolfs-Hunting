#ifndef WORLD_H
#define WORLD_H


#include <vector>
#include "Entity.h"
#include "stage.h"
#include "game.h"

class World
{
public:

	World();
	void render();
	void update();
	void renderFloor();
	void renderSky();
	bool checkCollision(Character * c, float dt);
	void World::renderTrees(std::vector<Matrix44> models[50][50], char* texture_name);

	~World();

	Mesh plane;
	std::vector<EntityTree> trees[50][50];
	std::vector<EntityHouse> houses[50][50];
	std::vector<Character> characters;
	std::vector<Matrix44> arbres[50][50], arbres_red[50][50], arbres_yellow[50][50];
	//std::vector<Animal> animals[50][50];
	std::vector<Animal> animals;
	//std::vector<Vector3> animal_pos[10][10]; //Dividim el mapa en troços de 10x10
	std::vector<EntityArrow> arrows;
	std::vector<Particle> particles;
};

#endif