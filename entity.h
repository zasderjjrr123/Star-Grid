#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sprite.h"
#include "particle.h"
#include <queue>

#ifndef ENTITY_H
#define ENTITY_H

// type of this entity (wip)
enum e_type {
	EMPTY,
	FIGHTER,
	EXPLOSION,
	ASTEROID,
	SHIP,
	EDITOR,
	LABEL
};

// abstract tile used for description of tiles
typedef struct f_tile {

	// name of the tile
	std::string name;
	// description of the tile
	std::string desc;
	// index of this tile on sprite sheet
	int index;
	// can this tile rotate?
	bool can_rotate;
	// max health of this tile
	int max_health;
	// the wieght of this time (kilos?)
	int weight;

} Tile_Desc;

class Entity {

	public:

		// constructor and destructor
		Entity(std::string path, std::string d_path, SDL_Renderer* gRenderer, std::queue<Entity*>* e_list, std::queue<Projectile*>* p_list, int height, int width){

			// initialize position as zero
			m_p_x = 0;
			m_p_y = 0;
			m_rot = 0;
			
			// initialize sprite
			m_vis = Sprite();
			m_sprite_state = m_vis.set_sprite_sheet(path, d_path, gRenderer);
			
			// initialize array as null
			m_array = nullptr;
			m_width = height;
			m_height = width;

			// allocate the array
			init_array();

			// give array to sprite
			m_vis.set_array(m_array, m_width, m_height);

			// set center and radius, and give to sprite
			m_center = {12 * m_width, 12 * m_height};
			m_radius = sqrt(m_center.x * m_center.x + m_center.y * m_center.y);
			m_vis.set_center(m_center);

			// set some other variables
			m_total = 0;
			m_dead = false;
			m_type = EMPTY;
			m_team = 0;

			// set entity and projectile list
			m_e_list = e_list;
			m_p_list = p_list;
			m_renderer = gRenderer;
		}

		virtual ~Entity(){}

		// allocate space for the array
		void init_array(){

			// if there is no array, dont allocate it
			if (m_width == 0 || m_height == 0){
				return;
			}

			// allocate row
			m_array = static_cast<Tile**>(calloc(m_width,sizeof(Tile*)));

			// allocate columns
			for (int i = 0; i < m_width; i++){
				m_array[i] = static_cast<Tile*>(calloc(m_height,sizeof(Tile)));
			}

		}

		// given an array of key states, do stuff (nothing by default)
		virtual void check_keys(const Uint8* key_states){return;}

		// update velocity / acceleration / position
		virtual void update() = 0;

		// update the sin and cosine of m_rot
		void update_trig(){
			double rad_rot = m_rot * RAD_CONST;
			m_sin_rad = sin(rad_rot);
			m_cos_rad = cos(rad_rot);	
		}

		// rotate the given array index i, j to find point in global space.
		// pass doubles by reference
		void rotate_e_to_g(int i, int j, double& d_x, double& d_y){
			
			// get our point in a grid
			double p_x = static_cast<double>((i * SPRITE_SIZE - m_center.x)) + 12;
			double p_y = static_cast<double>((m_center.y - j * SPRITE_SIZE)) - 12;

			// rotate
			d_x = m_p_x - p_x * m_sin_rad - p_y * m_cos_rad;
			d_y = m_p_y - p_y * m_sin_rad + p_x * m_cos_rad;
		}

		// given a point in global space, find the tile index
		// pass ints by reference
		void rotate_g_to_e(double d_x, double d_y, int& i, int& j){

			d_x -= static_cast<double>(m_p_x);
			d_y = static_cast<double>(m_p_y) - d_y;
			

			j = (static_cast<int>(d_y * m_sin_rad + d_x * m_cos_rad) + m_center.x) / SPRITE_SIZE;
			i = (static_cast<int>(d_x * m_sin_rad - d_y * m_cos_rad) + m_center.y) / SPRITE_SIZE;

		}

		// how much health is at given position (in case of solid entity (see fighter))
		virtual int get_health(int i, int j) {return m_array[i][j].health;};

		// deal given damage to given module (also double coords of collision if need for particles)
		virtual void damage_module(int i, int j, int damage, double d_i, double d_j) {m_array[i][j].health -= damage;}

		// draw to screen (and anything else)
		virtual void draw(double,double) = 0;

		// given a collision at given place, with given damage, return
		// returned damage (can be greater than damage given)
		// !!!IMPORTANT!!! Damage is a unit of energy 
		virtual int collide(double d_x, double d_y, int damage) {return 0;}

		// check for every position, whether we are colliding with an entity
		void check_collisions(){

			// if we are missing a list, return
			if (m_e_list == nullptr || m_p_list == nullptr) {
				return;
			}

			// pointer for and legnth of entity list
			int e_len;
			Entity* temp_e;
			// pointer for and length of projectile list
			int p_len;
			Projectile* temp_p;
			
			// temporary health for projectiles
			int temp_h;

			double d_x, d_y;

			// hitboxes
			SDL_Rect r;
			r.h = 24;
			r.w = 24;

			SDL_SetRenderDrawColor(m_renderer,0x00,0x00,0xFF,0xFF);

			for (int i = 0; i < m_width; i++) {
				for (int j = 0; j < m_height; j++) {
		
					//if (m_array[i][j].indx == 0){
					//	continue;
					//}

					rotate_e_to_g(i, j, d_x, d_y);
					
					// hitboxes
					r.x = static_cast<int>(d_x + 0.5f) - 12;
					r.y = static_cast<int>(d_y + 0.5f) - 12;

					if (m_type != FIGHTER) {
						SDL_RenderDrawRect(m_renderer,&r);
					}

					e_len = static_cast<int>(m_e_list->size());
					// check for collisions with entities
					for (int k = 0; k < e_len; k++){
						// get entity from list
						temp_e = m_e_list->front();
						m_e_list->pop();

						// if it is close, perform collision
						if (is_close(temp_e, d_x, d_y) && get_health(i,j) > 0){
							damage_module(i,j,temp_e->collide(d_x,d_y,get_health(i,j)),d_x,d_y);
						}

						// put it back in list
						m_e_list->push(temp_e);
					}

					p_len = static_cast<int>(m_p_list->size());	
					// check for collisions with projectiles
					for (int k = 0; k < p_len; k++){

						// get projectile from list
						temp_p = m_p_list->front();
						m_p_list->pop();

						// if it is close, collide
						if (temp_p->team != m_team && is_close(temp_p,d_x,d_y) && get_health(i,j) > 0 && temp_p->damage > 0){
							if (get_health(i,j) >= temp_p->damage){
								damage_module(i,j,temp_p->damage,d_x,d_y);
								Particle_Engine::free_pr(temp_p);
								temp_p = nullptr;
							} else {
								temp_h = get_health(i,j);
								damage_module(i,j,temp_h,d_x,d_y);
								temp_p->damage -= temp_h;
							}
						}

						// if it still exists, put it back in list
						if (temp_p != nullptr){
							m_p_list->push(temp_p);
						}
					}	
				}
			}
		}
	
		// get either position of our entity		
		double get_position_x(){return m_p_x;}
		double get_position_y(){return m_p_y;}
		
		// returns true if this entity should be deleted
		bool is_dead(){return m_dead;}

		// get the type of this entity (see above struct)
		int get_type(){return m_type;}

		// gets the radius of this entity
		double get_radius(){return m_radius;}

		// find the center of mass for our asteroid
		SDL_Point center_of_mass(){
				
			double tot_x = 0;
			double tot_y = 0;
			double tot_m = 0;
			m_total = 0;

			// for every tile...
		       	for (int i = 0; i < m_width; i++){
				for (int j = 0; j < m_height; j++){
					// ...if we have a node, add weight...
					if (m_array[i][j].indx){
						tot_x += ((i + 0.5f) * m_array[i][j].weight * SPRITE_SIZE);
						tot_y += ((j + 0.5f) * m_array[i][j].weight * SPRITE_SIZE);
						tot_m += m_array[i][j].weight;
						m_total += 1;
					}
				}
			}

			// edge case for no mass
			if (tot_m == 0) {
				return SDL_Point {12 * m_width,12 * m_height};
			}

			// ...then divide
			return SDL_Point {static_cast<int>(tot_x / tot_m),static_cast<int>(tot_y / tot_m)};
		}

		// quick check to determine if the given entity is close to us
		static bool is_close(Entity* e, double p_x, double p_y){
			return (std::abs(e->get_position_x() - p_x) < (e->get_radius())) && (std::abs(e->get_position_y() - p_y) < (e->get_radius()));
		}
	

		// quick check to determine if projectile is close to us	
		static bool is_close(Projectile* p, double p_x, double p_y){
			return (std::abs(p->p->p_x - p_x) < (p->radius)) && (std::abs(p->p->p_y - p_y) < (p->radius));
		
		}

		// gets a random double between high and low
		static double rand_double(double high, double low){ 
			return (low + ((static_cast<double>(rand())) / (static_cast<double>((RAND_MAX)))) * (high - low));
		}

	protected:

		// multiple to go from degrees -> radians
		static constexpr double RAD_CONST = 3.14159265358979/180;
		// default radius in pixels (assume 24x24 ie 1 tile)
		static constexpr double DEFAULT_RADIUS = 16.97056275;
		// default center in pixels (assume 24x24 ie 1 tile)
		static constexpr SDL_Point DEFAULT_CENTER = {12,12};

		// the list of entities and projectiles to collide with (if we don't collide, set them to null)
		std::queue<Entity*>* m_e_list;
		std::queue<Projectile*>* m_p_list;

		SDL_Renderer* m_renderer;

		// what team is this entity on
		int m_team;

		// is this entity dead?
		bool m_dead;

		// what type of entity is this (ship, fighter, asteroid, etc)?
		int m_type;

		// the position of our entity
		double m_p_x;
		double m_p_y;
		double m_rot;

		// sin and cos of current m_rot (computed once per frame)
		double m_sin_rad;
		double m_cos_rad;

		// array, width, and height
		Tile** m_array;
		int m_width;
		int m_height;

		// the center and radius of our entity, for collision
		double m_radius;
		SDL_Point m_center;

		// the total number of tiles in our ship
		int m_total;

		// did the render work?
		int m_sprite_state;

		// the actual sprite that is drawn the the screen
		Sprite m_vis;


};

#endif
