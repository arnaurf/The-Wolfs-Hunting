#pragma once


#ifndef STAGE
#define STAGE

#include "includes.h"
#include "utils.h"
#include "camera.h"
#include <map>

#define FREGAMENT	0.95
#define ACC			11
#define ACC_FAST	18
#define ACC_SLOW	5

#define MAX_FOG		1200
#define MIN_FOG		100

#define MAX_RENDER	1000
#define MAX_HIT		600
#define MAP_SIZE	4000.0
#define MAX_HEIGHT	160.0
#define HEIGHT_FILE	"data/height_map_big.png"
#define COLOR_FILE	"data/color_map_big.png"

#define OFFSET_H 0
#define GAME_TIME 600
#define ANIM_OFFSET 2.5
class Stage {
public:
	static Stage* current; 
	static std::map<std::string, Stage*> stages;

	std::string name;
	long enter_time;

	Stage(const char* name);
	virtual void render() {}
	virtual void update(float dt, float time) {}
	virtual void onEnter() {}
		void reset_Game();
	static void changeStage(const char* name);
};

class IntroStage : public Stage {
public:
	IntroStage();

	virtual void render();
	virtual void update(float dt, float time);
};

class MenuStage : public Stage {
public:
	MenuStage();
	float height_arrow;
	virtual void render();
	virtual void update(float dt, float time);
};

class ControlsStage : public Stage {
public:
	ControlsStage();
	virtual void render();
	virtual void update(float dt, float time);
};

class PauseStage : public Stage {
public:
	PauseStage();
	float height_arrow;
	virtual void render();
	virtual void update(float dt, float time);
};

class EndingStage : public Stage {
public:
	EndingStage();

	virtual void render();
	virtual void update(float dt, float time);
};

class LoseStage : public Stage {
public:
	LoseStage();

	virtual void render();
	virtual void update(float dt, float time);
};

class GameStage : public Stage {
public:
	static GameStage* instance;

	GameStage();
	SDL_Window* window;
	int window_width;
	int window_height;

	//some globals
	long frame;
	float time;
	float t0;
	float remaining;
	float elapsed_time;
	int fps;
	bool must_exit;
	
	void drawGUI();

	void build_fences();

	//some vars
	Camera* camera; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)
	virtual void render();
	virtual void update(float dt, float time);
	

};

#endif 