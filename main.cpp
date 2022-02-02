#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <cmath>
#include <queue>
#include "fighter.h"
#include "editor.h"
#include "frame_counter.h"
#include "asteroid.h"

const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 900;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer; 

// initialize SDL
bool init() {
	// init random
	std::srand(time(0));

	//Initialize SDL, and return if failed
	if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return false;
       	} 
	
	// try to make the window, and return if failed
	gWindow = SDL_CreateWindow( "Fighter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if(gWindow == NULL) {
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// try to make renderer, and return if failed
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL){
		printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// set our renderer color and blend mode
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_SetRenderDrawBlendMode(gRenderer,SDL_BLENDMODE_BLEND);

	// try to load SDL_image
	int img_flags = IMG_INIT_PNG;
	if ( !(IMG_Init(img_flags) & img_flags)){
		printf("SDL_image error: %s\n",IMG_GetError());
		return false;
	}

	return true;

}

// close the SDL
void close() {
    
	// destroy window and render
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

// update all entities in given list
void update_entities(std::queue<Entity*>* e_list){
	// update entities
	int len = e_list->size();
	Entity* temp;
	for (int i = 0; i < len; i++){
		temp = e_list->front();
		e_list->pop();
		temp->update();
		if (temp->is_dead()){
			delete temp;
		} else { 
			e_list->push(temp);
		}
	}
}

// update all projectiles in given list
void update_projectiles(Particle_Engine* p_e, std::queue<Projectile*>* p_list){
	// update projectiles
	int len = p_list->size();
	Projectile* temp_p;
	for (int i = 0; i < len; i++){
		temp_p = p_list->front();
		p_list->pop();
		p_e->update_particle(temp_p->p);
		if (temp_p->p->is_dead){
			Particle_Engine::free_pr(temp_p);
		} else { 
			p_list->push(temp_p);
		}
	}
}

// update all particles in given list
void update_particles(Particle_Engine* p_e, std::queue<Particle*>* p_list){
	// update particles
	int len = p_list->size();
	Particle* temp_p;
	for (int i = 0; i < len; i++){
		temp_p = p_list->front();
		p_list->pop();
		p_e->update_particle(temp_p);
		if (temp_p->is_dead){
			Particle_Engine::free_p(temp_p);
		} else { 
			p_list->push(temp_p);
		}
	}
}


// render list of entities
void render_entities(std::queue<Entity*>* e_list, double tc_x, double tc_y){
	// render entities
	int len = e_list->size();
	Entity* temp;
	for (int i = 0; i < len; i++){
		temp = e_list->front();
		e_list->pop();
		temp->draw(tc_x,tc_y);
		e_list->push(temp);
	}
}

// render list of entities
void render_projectiles(Particle_Engine* p_e, std::queue<Projectile*>* p_list, double tc_x, double tc_y){
	// render projectiles
	int len = p_list->size();
	Projectile* temp_pr;
	for (int i = 0; i < len; i++){
		temp_pr = p_list->front();
		p_list->pop();
		p_e->draw_particle(temp_pr->p,tc_x,tc_y);
		p_list->push(temp_pr);
	}
}

// render list of entities
void render_particles(Particle_Engine* p_e, std::queue<Particle*>* p_list, double tc_x, double tc_y){
	// render particles
	int len = p_list->size();
	Particle* temp_p;
	for (int i = 0; i < len; i++){
		temp_p = p_list->front();
		p_list->pop();
		p_e->draw_particle(temp_p,tc_x,tc_y);
		p_list->push(temp_p);
	}
}

int main(int argc, char* args[]) {

	bool running = true;
	SDL_Event e;
	const Uint8* key_states;

	if (!init()){
		printf("Error Initializing!\n");
		return 1;
	}

	Frame_Counter timer = Frame_Counter(60);
	
	std::queue<Entity*> entity_list = {};
	
	// list for particles, and engine for particles
	std::queue<Particle*> particle_list = {};
	std::vector<std::string> p_file_list = {"general.png"};
	Particle_Engine particle_e = Particle_Engine(gRenderer,p_file_list,&timer);

	// list for projectiles
	std::queue<Projectile*> projectile_list = {};

	Editor* s = new Editor("ship",gRenderer,&timer,30,30);

	s->file_import();

	entity_list.push(s);
		
	//Fighter *f = new Fighter(gRenderer,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2,&timer,&entity_list,&projectile_list,&particle_list);

	//entity_list.push(f);

	//entity_list.push(new Asteroid(gRenderer, &entity_list, &particle_list, &projectile_list, 300, 300, 0.0, 0.0, 20, 20));

	// used for list iteration
	int len;
	Entity* temp;
	Particle* temp_p;
	Projectile* temp_pr;

	// top corner of the screen, changes with focus
	double tc_x = 0;
	double tc_y = 0;

	timer.start_counter();
	while(running) {
		while(SDL_PollEvent(&e) != 0){
			if (e.type == SDL_QUIT){
				running = false;
			}
			
			if (e.type == SDL_KEYDOWN){
				if (e.key.keysym.sym == SDLK_f){
					//s->file_export();
				}
			}
			
		}

		key_states = SDL_GetKeyboardState(NULL);

		len = entity_list.size();

		for (int i = 0; i < len; i++){
			temp = entity_list.front();
			entity_list.pop();
			temp->check_keys(key_states);
			entity_list.push(temp);
		}

		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		
		SDL_RenderClear(gRenderer);

		// update stuff
		update_entities(&entity_list);		
		update_projectiles(&particle_e,&projectile_list);
		update_particles(&particle_e,&particle_list);

		//tc_x = f->get_position_x() - SCREEN_WIDTH / 2;
		//tc_y = f->get_position_y() - SCREEN_HEIGHT / 2;

		// render stuff
		render_entities(&entity_list,tc_x,tc_y);
		render_projectiles(&particle_e,&projectile_list,tc_x,tc_y);
		render_particles(&particle_e,&particle_list,tc_x,tc_y);

		SDL_RenderPresent(gRenderer);

		timer.wait_for_next_frame();
	}

	// delete entities
	len = entity_list.size();
	for (int i = 0; i < len; i++){
		temp = entity_list.front();
		entity_list.pop();
		delete temp;
	}

	// delete projectiles
	len = projectile_list.size();
	for (int i = 0; i < len; i++){
		temp_pr = projectile_list.front();
		projectile_list.pop();
		Particle_Engine::free_pr(temp_pr);
	}

	// delete particles
	len = particle_list.size();
	for (int i = 0; i < len; i++){
		temp_p = particle_list.front();
		particle_list.pop();
		Particle_Engine::free_p(temp_p);
	}

	close();

	return 0;
}
