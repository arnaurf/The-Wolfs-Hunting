#include "stage.h"
#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
#include "World.h"
#include "Audio.h"

#include <cmath>
#include <list>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

Texture* texture = NULL;
Animation* anim = NULL;
Character* character = NULL;
float angle = 0;
Mesh* mesh = NULL;
Mesh plane;
Shader *shader = NULL, *mask = NULL, *tree = NULL, *shader_anim = NULL;
World world;
GameStage* GameStage::instance = NULL;
float y_camera;
Image h;
Image trees;
Image animal_map;
float is_near;
clock_t t1;
float is_running = 0;
bool map;
Audio *running, *shoot_arrow, *ambient;
bool is_playing1 = false, is_playing2 = false;
float contr_time = 0;
Stage* Stage::current = NULL;
std::map<std::string, Stage*> Stage::stages;
int points = 0;

Stage::Stage(const char* name)
{
	this->name = name;
	stages[name] = this;
}

void Stage::changeStage(const char* name)
{
	auto it = stages.find(name);
	if (it == stages.end())
		return;
	if (current == it->second)
		return;
	current = it->second;
	current->enter_time = getTime();
	current->onEnter();
}


//----INTRO STAGE

IntroStage::IntroStage() : Stage("intro")
{
	if (BASS_Init(0, 44100, 0, 0, NULL) == false)
	{
		printf("Error while opening the soung card\n");
	}

	running = new Audio(BASS_SampleLoad(false, "data/audio/running.wav", 0, 0, 3, 0));
	if (running->sample == 0) {
		printf("SoundFile not found");
	}

	shoot_arrow = new Audio(BASS_SampleLoad(false, "data/audio/shoot_arrow.wav", 0, 0, 3, 0));
	if (shoot_arrow->sample == 0) {
		printf("SoundFile not found");
	}

	ambient = new Audio(BASS_SampleLoad(false, "data/audio/ambient.wav", 0, 0, 3, BASS_SAMPLE_LOOP));
	if (ambient->sample == 0) {
		printf("SoundFile not found");
	}
}

void IntroStage::render()
{
	Camera cam2D;
	//cam2D.setOrthographic();

	glDisable(GL_DEPTH_TEST);		//desabilitar els dos pk no doni problemes
	glDisable(GL_CULL_FACE);

	Shader * sh_2d = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
	if (sh_2d) {
		sh_2d->enable();
		Mesh m;
		//m.createQuad();//ens creara un quadrat

		Matrix44 model; model.translate(0, 0, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/background_begin.png"));
		m.createQuad(0, 0, 2, 2, false);
		m.render(GL_TRIANGLES);

		sh_2d->setUniform("u_time", Game::instance->time);
		model; model.translate(0, -0.5, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);

		sh_2d->setUniform("u_texture", Texture::Get("data/press.png"));

		m.createQuad(0, 0, 0.5, 0.2, false);
		m.render(GL_TRIANGLES);

		
		sh_2d->disable();
	}
}

void IntroStage::update(float dt, float time)
{
	if (Input::wasKeyPressed(SDL_SCANCODE_X) || Input::gamepads->wasButtonPressed(A_BUTTON)) {
		Stage::changeStage("menu");
	}
}


//----MENU STAGE

MenuStage::MenuStage() : Stage("menu")
{
	height_arrow = 0;
}

void MenuStage::render()
{
	Camera cam2D;
	//cam2D.setOrthographic();

	glDisable(GL_DEPTH_TEST);		//desabilitar els dos pk no doni problemes
	glDisable(GL_CULL_FACE);

	Shader * sh_2d = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
	if (sh_2d) {
		sh_2d->enable();
		Mesh m;
		//m.createQuad();//ens creara un quadrat

		sh_2d->setUniform("u_time", Game::instance->time);

		Matrix44 model; model.translate(0, 0, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/background.png"));
		m.createQuad(0, 0, 2, 2, false);
		m.render(GL_TRIANGLES);

		model.setIdentity(); model.translate(0, -0.2, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/sg.png"));
		m.createQuad(0, 0, 1, 1, false);
		m.render(GL_TRIANGLES);

		model.setIdentity(); model.translate(-0.55, height_arrow, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/arrow.png"));
		m.createQuad(0, 0, 0.17, 0.17, false);
		m.render(GL_TRIANGLES);

				
		sh_2d->disable();
	}
}

void MenuStage::update(float dt, float time)
{
	//printf("%f\n", height_arrow);
	
	if (Input::wasKeyPressed(SDL_SCANCODE_UP) || Input::gamepads->wasButtonPressed(Y_BUTTON)) {
		if(height_arrow<-0.1)
			height_arrow += 0.2;
		
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_DOWN) || Input::gamepads->wasButtonPressed(X_BUTTON)) {
		if (height_arrow>-0.4)
			height_arrow -= 0.2;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_RETURN)||Input::gamepads->wasButtonPressed(A_BUTTON)) {
		if (height_arrow < -0.19 && height_arrow > -0.21) {
			Stage::changeStage("controls");
		}
		else {
			Stage::changeStage("game");
		}
	}
}

//----CONTROLS STAGE
ControlsStage::ControlsStage() : Stage("controls")
{
}

void ControlsStage::render()
{
	Camera cam2D;
	//cam2D.setOrthographic();

	glDisable(GL_DEPTH_TEST);		//desabilitar els dos pk no doni problemes
	glDisable(GL_CULL_FACE);

	Shader * sh_2d = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
	if (sh_2d) {
		sh_2d->enable();
		Mesh m;
		//m.createQuad();//ens creara un quadrat

		sh_2d->setUniform("u_time", Game::instance->time);

		Matrix44 model; model.translate(0, 0, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/gamepad.png"));
		m.createQuad(0, 0, 2, 2, false);
		m.render(GL_TRIANGLES);

		sh_2d->disable();
	}
}

void ControlsStage::update(float dt, float time)
{
	if (Input::wasKeyPressed(SDL_SCANCODE_RETURN) || Input::gamepads->wasButtonPressed(B_BUTTON)) {
		Stage::changeStage("menu");
	}
}


//----PAUSE STAGE
PauseStage::PauseStage() : Stage("pause")
{
	height_arrow = -0.2;
}

void PauseStage::render()
{
	if (is_playing2 == true) {
		is_playing2 = false;
		BASS_SampleStop(ambient->sample);
	}
	Camera cam2D;
	//cam2D.setOrthographic();

	glDisable(GL_DEPTH_TEST);		//desabilitar els dos pk no doni problemes
	glDisable(GL_CULL_FACE);

	Shader * sh_2d = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
	if (sh_2d) {
		sh_2d->enable();
		Mesh m;
		//m.createQuad();//ens creara un quadrat

		sh_2d->setUniform("u_time", Game::instance->time);

		Matrix44 model; model.translate(0, 0, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/background.png"));
		m.createQuad(0, 0, 2, 2, false);
		m.render(GL_TRIANGLES);

		model.setIdentity(); model.translate(0, -0.2, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/pause.png"));
		m.createQuad(0, -0.2, 1, 1, false);
		m.render(GL_TRIANGLES);

		model.setIdentity(); model.translate(-0.55, height_arrow, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/arrow.png"));
		m.createQuad(0, height_arrow, 0.17, 0.17, false);
		m.render(GL_TRIANGLES);


		sh_2d->disable();
	}
}

void PauseStage::update(float dt, float time)
{
	//printf("%f\n", height_arrow);

	if (Input::wasKeyPressed(SDL_SCANCODE_UP) || Input::gamepads->wasButtonPressed(Y_BUTTON)) {
		if (height_arrow<-0.2)
			height_arrow += 0.1;

	}
	if (Input::wasKeyPressed(SDL_SCANCODE_DOWN) || Input::gamepads->wasButtonPressed(X_BUTTON)) {
		if (height_arrow>-0.4)
			height_arrow -= 0.1;
	}
	
	if (Input::wasKeyPressed(SDL_SCANCODE_RETURN) || Input::gamepads->wasButtonPressed(A_BUTTON)) {
		if (height_arrow < -0.19 && height_arrow > -0.21) {
			Stage::changeStage("game");
		}
		else {
			Stage::changeStage("menu");
			reset_Game();

		}
	}
}
//----END STAGE

EndingStage::EndingStage() : Stage("ending")
{
}

void EndingStage::render()
{
	if (is_playing2 == true) {
		is_playing2 = false;
		BASS_SampleStop(ambient->sample);
	}
	Shader * sh_2d = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
	if (sh_2d) {
		sh_2d->enable();
		Mesh m;
		//m.createQuad();//ens creara un quadrat

		sh_2d->setUniform("u_time", Game::instance->time);

		Matrix44 model; model.translate(0, 0, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/background_end.png"));
		m.createQuad(0, 0, 2, 2, false);
		m.render(GL_TRIANGLES);
		
		sh_2d->disable();
	}

}

void EndingStage::update(float dt, float time)
{
	if (Input::wasKeyPressed(SDL_SCANCODE_RETURN) || Input::gamepads->wasButtonPressed(A_BUTTON)) {
		Stage::changeStage("intro");
	}
}

LoseStage::LoseStage() : Stage("lose")
{
}

void LoseStage::render()
{
	if (is_playing2 == true) {
		is_playing2 = false;
		BASS_SampleStop(ambient->sample);
	}
	Shader * sh_2d = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
	if (sh_2d) {
		sh_2d->enable();
		Mesh m;
		//m.createQuad();//ens creara un quadrat

		sh_2d->setUniform("u_time", Game::instance->time);

		Matrix44 model; model.translate(0, 0, 0); model.rotate(0, Vector3(0, 0, 1));
		sh_2d->setUniform("u_model", model);
		sh_2d->setUniform("u_texture", Texture::Get("data/background_end_lose.png"));
		m.createQuad(0, 0, 2, 2, false);
		m.render(GL_TRIANGLES);

		sh_2d->disable();
	}

}

void LoseStage::update(float dt, float time)
{
	if (Input::wasKeyPressed(SDL_SCANCODE_RETURN) || Input::gamepads->wasButtonPressed(A_BUTTON)) {
		Stage::changeStage("intro");
	}
}



//----GAME STAGE

void add_trees(Image& tree_map, Image& animal_map) {


	for (int i = 0; i < tree_map.width; i++) {
		for (int j = 0; j < tree_map.height; j++) {
			Color a = tree_map.getPixel(i, j);
			Color b = animal_map.getPixel(i, j);
			float y = h.getPixelInterpolated(i / (float)tree_map.width*h.width, j / (float)tree_map.height * h.height).x;

			float xx = i / (float)tree_map.width * MAP_SIZE;
			float yy = j / (float)tree_map.height * MAP_SIZE;

			if (a.g == 255) {
				Matrix44 m;
				m.translate(yy, y / 255.0f * MAX_HEIGHT, xx);

				int rando = rand();
				m.scale(6 + (rand() % 100) / 20, 6 + (rand() % 100) / 20, 6 + (rand() % 100) / 20);
				m.rotate((rando % 100) / 200.0 * PI, Vector3(0, 1, 0));

				int y = (int)(yy / MAP_SIZE * 49);
				int x = (int)(xx / MAP_SIZE * 49);
				world.arbres[y][x].push_back(m);
			}
			if (a.b == 255) {
				Matrix44 m;
				m.translate(yy, y / 255.0f * MAX_HEIGHT, xx);

				int rando = rand();
				m.scale(6 + (rand() % 100) / 20, 6 + (rand() % 100) / 20, 6 + (rand() % 100) / 20);
				m.rotate((rando % 100) / 200.0 * PI, Vector3(0, 1, 0));


				int y = (int)(yy / MAP_SIZE * 49);
				int x = (int)(xx / MAP_SIZE * 49);
				world.arbres_yellow[y][x].push_back(m);
			}
			if (a.r == 255) {
				Matrix44 m;
				m.translate(yy, y / 255.0f * MAX_HEIGHT, xx);
				int rando = rand();
				m.scale(6 + (rand() % 100) / 20, 6 + (rand() % 100) / 20, 6 + (rand() % 100) / 20);
				m.rotate((rando % 100) / 200.0 * PI, Vector3(0, 1, 0));

				int y = (int)(yy / MAP_SIZE * 49);
				int x = (int)(xx / MAP_SIZE * 49);
				world.arbres_red[y][x].push_back(m);
			}


			if (rand() % 2000 < b.r / 1200.0) {
				//world.animal_pos[int(yy / MAP_SIZE * 10)][int(xx / MAP_SIZE * 10)].push_back(Vector3(yy, y / 255.0f * MAX_HEIGHT, xx));
				//world.animals[int(yy/MAP_SIZE*49)][int(xx / MAP_SIZE * 49)].push_back(Animal(Vector3(yy, y / 255.0f * MAX_HEIGHT + 5, xx), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));
				world.animals.push_back(Animal(Vector3(yy, y / 255.0f * MAX_HEIGHT + ANIM_OFFSET, xx), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));

			}
			if (rand() % 255 < b.g / 600.0) {
				//world.animal_pos[int(yy / MAP_SIZE * 10)][int(xx / MAP_SIZE * 10)] = Vector3(yy, y / 255.0f * MAX_HEIGHT, xx);

				//world.animals.push_back(Animal(Vector3(yy, y / 255.0f * MAX_HEIGHT, xx), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));
			}
			if (rand() % 255 < b.b / 600.0) {
				//world.animal_pos[int(yy / MAP_SIZE * 10)][int(xx / MAP_SIZE * 10)] = Vector3(yy, y / 255.0f * MAX_HEIGHT, xx);
				//world.animals.push_back(Animal(Vector3(yy, y / 255.0f * MAX_HEIGHT, xx), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));
			}


		}
	}

	//world.trees
}


void add_animals_only(Image& animal_map) {


	for (int i = 0; i < animal_map.width; i++) {
		for (int j = 0; j < animal_map.height; j++) {
			Color b = animal_map.getPixel(i, j);
			float y = h.getPixelInterpolated(i / (float)animal_map.width*h.width, j / (float)animal_map.height * h.height).x;

			float xx = i / (float)animal_map.width * MAP_SIZE;
			float yy = j / (float)animal_map.height * MAP_SIZE;

			if (rand() % 2000 < b.r / 1200.0) {
				//world.animal_pos[int(yy / MAP_SIZE * 10)][int(xx / MAP_SIZE * 10)].push_back(Vector3(yy, y / 255.0f * MAX_HEIGHT, xx));
				//world.animals[int(yy/MAP_SIZE*49)][int(xx / MAP_SIZE * 49)].push_back(Animal(Vector3(yy, y / 255.0f * MAX_HEIGHT + 5, xx), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));
				world.animals.push_back(Animal(Vector3(yy, y / 255.0f * MAX_HEIGHT + ANIM_OFFSET, xx), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));

			}
			if (rand() % 255 < b.g / 600.0) {
				//world.animal_pos[int(yy / MAP_SIZE * 10)][int(xx / MAP_SIZE * 10)] = Vector3(yy, y / 255.0f * MAX_HEIGHT, xx);

				//world.animals.push_back(Animal(Vector3(yy, y / 255.0f * MAX_HEIGHT, xx), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));
			}
			if (rand() % 255 < b.b / 600.0) {
				//world.animal_pos[int(yy / MAP_SIZE * 10)][int(xx / MAP_SIZE * 10)] = Vector3(yy, y / 255.0f * MAX_HEIGHT, xx);
				//world.animals.push_back(Animal(Vector3(yy, y / 255.0f * MAX_HEIGHT, xx), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));
			}


		}
	}

	//world.trees
}


GameStage::GameStage() : Stage("game")
{
	instance = this;
	must_exit = false;
	//h.fromTexture(Texture::Get("data/height.tga"));
	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;

	mouse_locked = true;
	h.loadPNG(HEIGHT_FILE);
	t1 = clock();

	map = 0;
	//OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

							 //create our camera
	camera = new Camera();
	//camera->lookAt(Vector3(200.f,100.f, 500.f),Vector3(500.f,0.f,500.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	mesh = Mesh::Get("data/town/house.obj");
	//plane.createSubdividedPlane(1000, 256, false); 
	Texture::Get("data/map.png");

	// example of shader loading using the shaders manager
	shader_anim = Shader::Get("data/shaders/anim.vs", "data/shaders/texture.fs");
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	mask = Shader::Get("data/shaders/mask.vs", "data/shaders/mask.fs");
	tree = Shader::Get("data/shaders/trees.vs", "data/shaders/trees.fs");



	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
								   //readFile("data/input.txt", 0);
								  // readFile("data/trees.txt", 1);

	//trees.loadTGA("data/prova.tga");
	srand(clock());
	animal_map.loadPNG("data/animal_mapping.png");
	trees.loadPNG("data/tree_map_dense2.png");
	add_trees(trees, animal_map);
	points = world.animals.size();

	//world.characters.push_back(Character(Vector3(500, 50, 500), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));
	world.characters.push_back(Character(Vector3(520, 35, 500), shader_anim, Mesh::Get("data/pj/character.mesh"), Texture::Get("data/pj/archer1_color.tga")));
	character = &world.characters.front();
	Character::active_character = character;
	//world.animals[int(character->position.x/MAP_SIZE*49)][int(character->position.z / MAP_SIZE * 49)].push_back(Animal(Vector3(character->position.x - 50, 50, character->position.z - 10), shader_anim, Mesh::Get("data/male.mesh"), Texture::Get("data/male.tga")));

	build_fences();

	int x = 500; int z = 800;
	int y = h.getPixelInterpolated(z / MAP_SIZE * 512, x / MAP_SIZE * 512).x / 255.0f * MAX_HEIGHT;
	Matrix44 m; m.translate(x, y, z); m.scale(1.5, 1.5, 1.5);
	world.houses[int(x/MAP_SIZE*49)][int(z/MAP_SIZE*49)].push_back(EntityHouse(m, tree, Mesh::Get("data/town/house.obj"), Texture::Get("data/town/house.tga"))); 

	Mesh::Get("data/arrow.obj");
	Matrix44 mm;
	mm.translate(250, 100, 250);
	mm.scale(6, 6, 6);
	t0 = 0;
	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
	Input::centerMouse();
	/*
	y_camera = -1000;
	Matrix44 mod = character->model;
	camera->eye = mod.getTranslation() + mod.rotateVector(Vector3(0, 30, -20));
	camera->center = camera->eye + mod.rotateVector(Vector3(0, -tan(y_camera * DEG2RAD), 1));
	*/
	camera->center = character->position + Vector3(0, 9, 0);
	camera->eye = character->position + Vector3(0, 12, -15);
}

void GameStage::render()
{
	if (is_playing2 == false) {
		ambient->channel = ambient->play(5);
		is_playing2 = true;
	}
	;	if (character->v.length() > 0.7) {
		if (is_playing1 == false) {
			is_playing1 = true;
			running->channel = running->play(2.5);

		}
	}
	else if (character->v.length() < 0.7) {
		BASS_SampleStop(running->sample);
		is_playing1 = false;
	}
	if(!t0) t0 = getTime();
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//create model matrix for cube	

	world.render();

	//printf("-%.f---%.f-\n", character->position.x, character->position.z);
	//render the FPS, Draw Calls, etc
	//drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	remaining = GAME_TIME - (getTime() - t0) / 1000.0;
	if (world.animals.size()==0) {
		Stage::changeStage("ending");
		reset_Game();
	}
	if (remaining <= 0) {
		Stage::changeStage("lose");
		reset_Game();
	}
	drawText(2, 2, "Remaining time: ", Vector3(1, 1, 1), 2);
	drawText(160, 2, std::to_string( int(remaining) ), Vector3(1, 1, 1), 2);
	drawText(300, 2, "Points: ", Vector3(1, 1, 1), 2);
	drawText(370, 2, std::to_string( points - world.animals.size() ), Vector3(1, 1, 1), 2);


	drawGUI();

	

	//swap between front buffer and back buffer
	
}

void GameStage::update(float seconds_elapsed, float time)
{
	float speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant
	bool collision = world.checkCollision(character, seconds_elapsed);
	//world.checkCollision(character, seconds_elapsed);
										 //example
	angle += (float)seconds_elapsed * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		//camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
		//camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
	}

	Matrix44 model = character->model;
	//camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));

	//CODI DEL GAMEPAD



	//async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) || Input::gamepads->isButtonPressed(A_BUTTON))
	{
		character->acc = ACC_FAST;
		is_running += 0.05;
	}
	else if (Input::isKeyPressed(SDL_SCANCODE_LCTRL)) {
		character->acc = ACC_SLOW;
	}
	else { character->acc = ACC; } // speed *= 10; //move faster with left shift

	if (abs(Input::gamepads->axis[LEFT_ANALOG_Y]) > 0.15) {
		//world.checkCollision(character, seconds_elapsed);
		//camera->center = camera->center + (character->position - character->last_position);
		character->v.y += -Input::gamepads->axis[LEFT_ANALOG_Y] * 0.013 * character->acc;
	}
	if (abs(Input::gamepads->axis[LEFT_ANALOG_X])>0.15)
		character->yaw += Input::gamepads->axis[LEFT_ANALOG_X];

	if (abs(Input::gamepads->axis[RIGHT_ANALOG_Y]) > 0.15) {
		camera->rotate(Input::gamepads->axis[RIGHT_ANALOG_Y] * 0.015f, camera->getLocalVector(Vector3(1.0f, 0.0f, 0.0f)));
	}
	if (abs(Input::gamepads->axis[RIGHT_ANALOG_X]) > 0.15) {
		/*
		camera->rotCamera += Input::gamepads->axis[RIGHT_ANALOG_X] * 0.015;
		Vector3 rot= Vector3(cos(camera->rotCamera)*0.3 - character->position.x, 0, sin(camera->rotCamera)*0.3 - character->position.z);
		camera->eye.x = camera->eye.x + (character->position.x + rot.x);
		camera->eye.z = camera->eye.z + (character->position.z + rot.z);
		*/
	}
		
	if (Input::gamepads->isButtonPressed(TRIGGER_RIGHT)) {
		character->v.y = 5.5;
	}
	/*
	if (Input::gamepads->isButtonPressed(LB_BUTTON)) {
		if (character->anim_mode != 1) {
			character->anim_mode = 1;
			character->last_time = time;

		}
		is_near += 0.13;
	}
	else if (character->anim_mode == 1 && time-contr_time > 1) {
		contr_time = time;
		character->anim_mode = 2;
		character->last_time = time;
		Matrix44 rot;
		rot.rotate(PI / 4, Vector3(0, 0, 1));
		rot.scale(0.07, 0.07, 0.07);
		Matrix44 full = rot * character->anim->skeleton.getBoneMatrix("mixamorig_LeftHandThumb3", false) * character->model;
		Matrix44 cam; cam.rotate(character->yaw*DEG2RAD, Vector3(0, 1, 0)); cam.rotate(-y_camera * DEG2RAD, Vector3(1, 0, 0));
		EntityArrow* arrow = new EntityArrow(full, Shader::Get("data/shaders/trees.vs", "data/shaders/trees.fs"), Mesh::Get("data/arrow.obj"),
			Texture::Get("data/black.tga"), time, Vector2((character->yaw - 90)*DEG2RAD, -y_camera * DEG2RAD));
		printf("\n Created arrow with yaw.y %f", -y_camera * DEG2RAD);
		arrow->v = cam.rotateVector(Vector3(0, 0, 3));
		world.arrows.push_back(*arrow);

		shoot_arrow->channel = shoot_arrow->play(4);
	}*/
	character->time = time;
	

	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
		character->v.x += character->acc *  seconds_elapsed * 0.7;
		//character->yaw -= 60 * elapsed_time;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
		character->v.x -= character->acc * seconds_elapsed * 0.7;
		//character->yaw += 60 * elapsed_time;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_E) ){
		Stage::changeStage("ending");
	}
	if (Input::isKeyPressed(SDL_SCANCODE_L)) {
		Stage::changeStage("lose");
	}

	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) {
		character->v.y += character->acc * seconds_elapsed;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) {
		character->v.y -= character->acc * seconds_elapsed;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_M) || Input::gamepads->wasButtonPressed(BACK_BUTTON)) {
		map = map != true;

	}
	if (Input::wasKeyPressed(SDL_SCANCODE_SPACE) || Input::gamepads->wasButtonPressed(START_BUTTON)) {
		Stage::changeStage("pause");
	}

	
	if (Input::isMousePressed(SDL_BUTTON_LEFT)) {

		if (character->anim_mode != 1) {
			character->anim_mode = 1;
			character->last_time = time;

		}
		is_near += 0.13;
	}
	else if (character->anim_mode == 1) {
		//contr_time = time;
		character->anim_mode = 2;
		character->last_time = time;
		Matrix44 rot;
		rot.rotate(PI / 4, Vector3(0, 0, 1));
		rot.scale(0.07, 0.07, 0.07);
		Matrix44 full = rot * character->anim->skeleton.getBoneMatrix("mixamorig_LeftHandThumb3", false) * character->model;
		Matrix44 cam; cam.rotate(character->yaw*DEG2RAD, Vector3(0, 1, 0)); cam.rotate(-y_camera * DEG2RAD, Vector3(1, 0, 0));
		EntityArrow* arrow = new EntityArrow(full, Shader::Get("data/shaders/trees.vs", "data/shaders/trees.fs"), Mesh::Get("data/arrow.obj"),
			Texture::Get("data/black.tga"), time, Vector2((character->yaw - 90)*DEG2RAD, -y_camera*DEG2RAD));
		printf("\n Created arrow with yaw.y %f", -y_camera*DEG2RAD);
		arrow->v = cam.rotateVector(Vector3(0, 0, 3));
		world.arrows.push_back(*arrow);

		shoot_arrow->channel = shoot_arrow->play(4);
	}
	character->time = time;

	for (int i = 0; i < world.arrows.size(); i++) {
		world.arrows[i].update(time);
	}
		

	
	//world.checkCollision(character, elapsed_time);
	for (std::vector<Character>::iterator it = world.characters.begin(); it != world.characters.end(); it++) {
		it->update(seconds_elapsed);
		it->time = time;
	}


	std::vector<std::vector<Animal>::iterator> to_erase;
	for (std::vector<Animal>::iterator it = world.animals.begin(); it != world.animals.end(); it++) {
		if (it->position.distance(character->position) < MAX_HIT){
			it->time = time;
			it->update(character, world.arbres[int(it->position.x / MAP_SIZE * 49)][int(it->position.z / MAP_SIZE * 49)], world.houses[int(it->position.x / MAP_SIZE * 49)][int(it->position.z / MAP_SIZE * 49)], seconds_elapsed, t1);
			
			Vector3 collision_point;
			Vector3 collision_normal;
			for (std::vector<EntityArrow>::iterator it2 = world.arrows.begin(); it2 != world.arrows.end(); it2++) {
				if (it->mesh->testSphereCollision(it->model, it2->pos, 4, collision_point, collision_normal)) {
					to_erase.push_back(it);
					world.particles.push_back(Particle(it->position, time));
				}
			}

		}
	}
	for (int i = 0; i < to_erase.size(); i++) {
		world.animals.erase(to_erase.at(i));
	}

	for (int i = 0; i < world.particles.size(); i++) {
		world.particles[i].update(time, seconds_elapsed);
	}

	//La camara estarà a Vector3(0,30,-20) respecte la direcció del vector del personatge (mod.rotateVector)
	//El fem mirar a una certa distancia.
	if (y_camera == -1000) {
		y_camera = 30;
	}
	else {
		float y_camera_new = y_camera - Input::mouse_delta.y * 0.07f;
		//printf("\n %f", y_camera);
		if (y_camera_new > -30 && y_camera_new < 40)
			y_camera = y_camera_new;
	}
	//GAMEPAD
	//camera->eye = camera->eye + (character->position - character->last_position);



	Matrix44 mod = character->model;
	Camera::last_enabled->eye = mod.getTranslation() + mod.rotateVector(Vector3(-0.15 - is_near * 0.05, 2 + ((y_camera - 20) / 40.0), -2 + is_near + is_running));

	is_running = is_running * 0.95;
	is_near = is_near * 0.95;
	//printf("\n %f", Input::mouse_delta.y);
	Vector3 new_center = mod.rotateVector(Vector3(0, -tan(y_camera * DEG2RAD), 1));

	//camera->center = camera->center + (character->position - character->last_position);
	Camera::last_enabled->center = Camera::last_enabled->eye + new_center;
	//camera->rotate(, Vector3(-0.0f, 0.0f, 0.0f));

	if (collision != true) {
		character->yaw -= Input::mouse_delta.x * 0.1f;
	}
	

	//--------------------------


	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

void GameStage::drawGUI() {
	//pintar la informacio de pantalla. Sha de fer transformacions a 2D

	Camera cam2D;
	//cam2D.setOrthographic();

	glDisable(GL_DEPTH_TEST);		//desabilitar els dos pk no doni problemes
	glDisable(GL_CULL_FACE);

	Shader * sh_2d = Shader::Get("data/shaders/2d.vs", "data/shaders/2d.fs");
	if (sh_2d) {
		sh_2d->enable();
		Mesh m;
		Matrix44 model; model.translate(0.75, 0.6, 0); model.rotate(-character->yaw* DEG2RAD +  0.05, Vector3(0, 0, 1));
		//m.createQuad();//ens creara un quadrat
		sh_2d->setUniform("u_time", Game::instance->time);
		sh_2d->setUniform("u_texture", Texture::Get("data/compass.png"));
		sh_2d->setUniform("u_model", model);

		m.createQuad(0.0, 0.0, 0.5, 0.5, false);
		m.render(GL_TRIANGLES);
		

		if (map) {
			Mesh m;
			Matrix44 model; model.translate(0, 0, 0); model.rotate(0, Vector3(0, 0, 1));
			sh_2d->setUniform("u_time", Game::instance->time);
			sh_2d->setUniform("u_texture", Texture::Get("data/map.png"));
			sh_2d->setUniform("u_model", model);

			m.createQuad(0.0, 0.0, 2, 2, false);
			m.render(GL_TRIANGLES);
		}

		sh_2d->disable();
	}
	//fer les crides a la GPU (set uniform)
}

void GameStage::build_fences() {
	for (int i = 0; i < 4000; i += 30) {
		int x = i; int z = 0;
		int y = h.getPixelInterpolated(z / MAP_SIZE * 512, x / MAP_SIZE * 512).x / 255.0f * MAX_HEIGHT;
		Matrix44 m; m.translate(x, y, z);  m.scale(1.5, 1.5, 1.5);
		world.houses[int(x/MAP_SIZE * 49)][int(z/ MAP_SIZE * 49)].push_back(EntityHouse(m, tree, Mesh::Get("data/town/wall.obj"), Texture::Get("data/town/house.tga")));
	}
	for (int i = 0; i < 4000; i += 30) {
		int x = 0; int z = i;
		int y = h.getPixelInterpolated(z / MAP_SIZE * 512, x / MAP_SIZE * 512).x / 255.0f * MAX_HEIGHT;
		Matrix44 m;  m.translate(x, y, z);  m.rotate(90 * DEG2RAD, Vector3(0, 1, 0)); m.scale(1.5, 1.5, 1.5);
		world.houses[int(x/ MAP_SIZE * 49)][int(z/ MAP_SIZE * 49)].push_back(EntityHouse(m, tree, Mesh::Get("data/town/wall.obj"), Texture::Get("data/town/house.tga")));
	}
	for (int i = 0; i < 4000; i += 30) {
		int x = 4000; int z = i;
		int y = h.getPixelInterpolated(z / MAP_SIZE * 512, x / MAP_SIZE * 512).x / 255.0f * MAX_HEIGHT;
		Matrix44 m; m.translate(x, y, z);  m.rotate(180 * DEG2RAD, Vector3(0, 1, 0)); m.scale(1.5, 1.5, 1.5);
		world.houses[int(x/ MAP_SIZE * 49)][int(z/ MAP_SIZE * 49)].push_back(EntityHouse(m, tree, Mesh::Get("data/town/wall.obj"), Texture::Get("data/town/house.tga")));
	}
	for (int i = 0; i < 4000; i += 30) {
		int x = i; int z = 4000;
		int y = h.getPixelInterpolated(z / MAP_SIZE * 512, x / MAP_SIZE * 512).x / 255.0f * MAX_HEIGHT;
		Matrix44 m; m.translate(x, y, z);  m.rotate(270 * DEG2RAD, Vector3(0, 1, 0)); m.scale(1.5, 1.5, 1.5);
		world.houses[int(x/ MAP_SIZE * 49)][int(z/ MAP_SIZE * 49)].push_back(EntityHouse(m, tree, Mesh::Get("data/town/wall.obj"), Texture::Get("data/town/house.tga")));
	}
}

void Stage::reset_Game() {
	character->position = Vector3(520, 35, 500);
	world.animals.clear();
	add_animals_only(animal_map);
	is_playing1 = false, is_playing2 = false;
	points = world.animals.size();
	is_running = 0;
	world.arrows.clear();

}





