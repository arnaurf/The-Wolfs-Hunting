#include "stage.h"
#include "game.h"
#include <fstream>

#include <cmath>


//some globals

Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;

	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;

	new IntroStage();
	new MenuStage();
	new ControlsStage();
	new LoseStage();
	new PauseStage();
	new GameStage();
	new EndingStage();
	Stage::changeStage("intro");
}

//what to do when the image has to be draw
void Game::render(void)
{
	Stage::current->render();

	SDL_GL_SwapWindow(this->window);
}



void Game::update(double seconds_elapsed)
{
	Stage::current->update(seconds_elapsed, time);
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	//glViewport(width*0.5, 0, width*0.5, height);
	window_width = width;
	window_height = height;
}


