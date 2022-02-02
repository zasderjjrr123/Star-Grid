#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "frame_counter.h"
#include "sprite.h"
#include <string>
#include <vector>

#ifndef PARTICLE_H
#define PARTICLE_H

// struct to use when you want to commit a particle
typedef struct particle_struct {
	
	// which sprite sheet are we using?
	short int sheet;
        // array of frames, length, and current frame
        short int* frames;
        short int frames_len;
        short int c_frame;
        // position and rotation
        double p_x;
        double p_y;
        double rot;
        // velocity and spin
        double v_x;
        double v_y;
        double v_r;
        // when it was created, how long its lives, is it dead
        Uint32 start_time;
        Uint32 lifetime;
        bool is_dead;

} Particle;

// Projectile struct, 
typedef struct projectile_struct {
	// the particle for rendering / position
	Particle* p;
	// what team is this projectile on
	int team;
	// how much damage the projectile will do
	int damage;
	// radius of this projectile
	double radius;
	// do we do sub particles?
	bool sub_particles;
	// how often to create them (out of 100)?
	short int sub_percent;
	// the sub particl to create (null if none)
	Particle* sub_p;
	// does this projectile explode?
	bool explode;
	// radius of explosion
	double e_radius;
	// damage of explosion
	int e_damage;

} Projectile;

enum sheet_list {
	PE_GENERAL
};

class Particle_Engine {

	public:
		// needs a list of files, and a counter for timing sprites / death
		Particle_Engine(SDL_Renderer* gRenderer, std::vector<std::string> file_list, Frame_Counter* m_counter);
		// destructor
		~Particle_Engine();
		// update the particle
		void update_particle(Particle *p);
		// draw the given particle to screen
		void draw_particle(const Particle *p, double tc_x, double tc_y);
		// allocate particle
		static Particle* p_alloc(int size);
		//allocate projectile
		static Projectile* pr_alloc(int size);
		// delete given particle
		static void free_p(Particle* p);
		// delete given projectile
		static void free_pr(Projectile* p);
	protected:
		
		Frame_Counter* m_counter;
		std::vector<Sprite*> m_vis;
		std::vector<Tile**> m_arrays;

};

#endif
