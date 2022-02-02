#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sprite.h"
#include "entity.h"
#include "frame_counter.h"
#include "particle.h"
#include <math.h>
#include <cmath>
#include <queue>

#ifndef FIGHTER_H
#define FIGHTER_H

class Fighter : public Entity{

	public:
		Fighter(SDL_Renderer* gRenderer, int screen_w, int screen_h, int p_x, int p_y, Frame_Counter* timer, std::queue<Entity*>* e_list, std::queue<Projectile*>* pr_list, std::queue<Particle*>* pt_list);
		void check_keys(const Uint8* key_states);
		void update();
		void draw(double tc_x, double tc_y);

		// if we collide, slow us down, and damage us
		int collide(double d_x, double d_y, int damage);
		// return our health, since we are solid
		int get_health(int i, int j) {return m_health;}
		// damage our health, since we are solid
		void damage_module(int i, int j, int damage, double d_i, double d_j){m_health -= damage;}

	protected:
		
		enum m_Sprite_sheet {
			F_AIR,
			F_FRONT_L,
			F_FRONT_R,
			F_FRONT_FIRING_L,
			F_FRONT_FIRING_R,
			F_BACK_L,
			F_BACK_R,
			F_BACK_ON_L,
			F_BACK_ON_R,
			F_BACK_BOOST_L,
			F_BACK_BOOST_R,
			F_FRONT_SHEILD_L,
			F_FRONT_SHEILD_R,
			F_BACK_SHEILD_L,
			F_BACK_SHEILD_R
		};

		// render the HUD for health, sheild, and boost;	
		void render_HUD();

		// apply forward thrust
		void forward(Uint8 boost);

		// add a particle to shoot out engine
		void add_engine_particle(int x, int y, double rot);

		// add damage particle
		void add_damage_particle(int x, int y);

		// check out bounds
		void bounds_check();

		// add particles upon death
		void death_particles();

		// shoot our weapons
		void fire_weapons();

		// list for adding particles
		std::queue<Particle*>* m_pt_list;

		// timer for particles and guns
		Frame_Counter* m_timer;

		// renderer for HUD
		SDL_Renderer* m_renderer;

		// screen width and height
		int m_screen_w;
		int m_screen_h;

		// health, damage state, boost, and recharge
		int m_health;
		int m_damage_state;
		int m_boost;
		bool m_recharge;

		// set our max health and boost, as well as mass
		const int m_max_health = 5000;
		const int m_max_boost = 200;
		const int m_boost_recharge = 160;
		const int m_mass = 20;

		//velocity
		double m_v_x;
		double m_v_y;

		// weapon cooldown
		const Uint32 m_cooldown = 200;

		// cooldown for the weapon spite
		const Uint32 m_sprite_cooldown = 25;

		// the time since last shot was taken
		Uint32 m_last_shot = 0;

};

#endif
