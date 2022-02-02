#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <queue>
#include "entity.h"
#include "particle.h"
#include "frame_counter.h"

#ifndef ASTEROID_H
#define ASTEROID_H

class Asteroid : public Entity {

	public:

		Asteroid(SDL_Renderer* gRenderer, std::queue<Entity*>* g_list, std::queue<Particle*>* particle_list, std::queue<Projectile*>* projectile_list, double p_x, double p_y, double rot, double rot_rate, int width, int height);
		void update();
		void draw(double tc_x, double tc_y);
		int collide(double d_x, double d_y, int damage);
		
		typedef struct A_tile {
			int low;
			int high;
			int rot;
		} Asteroid_t;

	private:

		// list for adding particles
		std::queue<Particle*>* m_pt_list;
		// rotation velocity
		double m_v_r;

		// asteroid pieces weight and health
		const int m_heavy_weight = 50;
		const int m_heavy_health = 30;
		const int m_light_weight = 20;
		const int m_light_health = 10;

		// collides the asteroid at x and y, deleting place and updating others
		void damage_module(int i, int j, int damage, double d_i, double d_j);

		// add debris
		void add_debris(double x, double y, bool heavy);

		// reference for positions on sprite sheet
		enum pieces {
			EMPTY,
			DEEP, 
			SHALLOW, 
			EDGE_SMALL_CRATER, 
			EDGE_SMALL_HILL, 
			EDGE_BIG_HILL,
			EDGE_BIG_CRATER, 
			CORNER_MID, 
			CORNER_LOW, 
			CORNER_HIGH,
			INNER_CORNER,
			DAUL_INNER_CORNER,
			TOP_DOME,
			TOP_SPIKE,
			LONE,
			CONNECTOR_THIN,
			CONNECTOR_THICK,
			QUAD_CORNER,
			T_EDGE_SMALL_CRATER,
			T_EDGE_SMALL_HILL,
			T_EDGE_BIG_HILL,
			T_EDGE_BIG_CRATER,
			T_CORNER_MID,
			T_CORNER_LOW,
			T_CORNER_HIGH,
			LC_EDGE_SMALL_CRATER, 
			LC_EDGE_SMALL_HILL, 
			LC_EDGE_BIG_HILL,
			LC_EDGE_BIG_CRATER, 
			RC_EDGE_SMALL_CRATER, 
			RC_EDGE_SMALL_HILL, 
			RC_EDGE_BIG_HILL,
			RC_EDGE_BIG_CRATER,
			TRIPLE_CORNER,
			CROSS_CORNER	
		};

		// gets a random int in range [min-max]
		int get_rand_int(int min, int max);

		// gets a random point at least 1/2 r away 
		// from center of circle (pass by reference)
		void get_rand_outer_half(int x, int y, int r, int &x2, int &y2);

		// returns an array of given size that shapes an asteroid
		// the integers are indexes into the enumerate above
		void set_asteroid();

		// get an array that is just the shape of the asteroid, without
		// integers to know tiles
		void get_asteroid_shape();

		// put a cirle of 1's at the given position and radius
		void put_circle(int x, int y, int r);

		// stores the asteroid tiles
		const Asteroid_t EDGE_0 = {.low = EDGE_SMALL_CRATER, .high = EDGE_BIG_CRATER, .rot = 0};
		const Asteroid_t EDGE_1 = {.low = EDGE_SMALL_CRATER, .high = EDGE_BIG_CRATER, .rot = 1};
		const Asteroid_t EDGE_2 = {.low = EDGE_SMALL_CRATER, .high = EDGE_BIG_CRATER, .rot = 2};
		const Asteroid_t EDGE_3 = {.low = EDGE_SMALL_CRATER, .high = EDGE_BIG_CRATER, .rot = 3};
		const Asteroid_t CORNER_0 = {.low = CORNER_MID, .high = CORNER_HIGH, .rot = 0};
		const Asteroid_t CORNER_1 = {.low = CORNER_MID, .high = CORNER_HIGH, .rot = 1};
		const Asteroid_t CORNER_2 = {.low = CORNER_MID, .high = CORNER_HIGH, .rot = 2};
		const Asteroid_t CORNER_3 = {.low = CORNER_MID, .high = CORNER_HIGH, .rot = 3};
		const Asteroid_t I_CORNER_0 = {.low = INNER_CORNER, .high = INNER_CORNER, .rot = 0};
		const Asteroid_t I_CORNER_1 = {.low = INNER_CORNER, .high = INNER_CORNER, .rot = 1};
		const Asteroid_t I_CORNER_2 = {.low = INNER_CORNER, .high = INNER_CORNER, .rot = 2};
		const Asteroid_t I_CORNER_3 = {.low = INNER_CORNER, .high = INNER_CORNER, .rot = 3};
		const Asteroid_t D_I_CORNER_0 = {.low = DAUL_INNER_CORNER, .high = DAUL_INNER_CORNER, .rot = 0};
		const Asteroid_t D_I_CORNER_1 = {.low = DAUL_INNER_CORNER, .high = DAUL_INNER_CORNER, .rot = 1};
		const Asteroid_t D_I_CORNER_2 = {.low = DAUL_INNER_CORNER, .high = DAUL_INNER_CORNER, .rot = 2};
		const Asteroid_t D_I_CORNER_3 = {.low = DAUL_INNER_CORNER, .high = DAUL_INNER_CORNER, .rot = 3};
		const Asteroid_t TOP_0 = {.low = TOP_DOME, .high = TOP_SPIKE, .rot = 0};
		const Asteroid_t TOP_1 = {.low = TOP_DOME, .high = TOP_SPIKE, .rot = 1};
		const Asteroid_t TOP_2 = {.low = TOP_DOME, .high = TOP_SPIKE, .rot = 2};
		const Asteroid_t TOP_3 = {.low = TOP_DOME, .high = TOP_SPIKE, .rot = 3};
		const Asteroid_t LONE_0 = {.low = LONE, .high = LONE, .rot = 0};
		const Asteroid_t CONNECTOR_0 = {.low = CONNECTOR_THIN, .high = CONNECTOR_THICK, .rot = 0};
		const Asteroid_t CONNECTOR_1 = {.low = CONNECTOR_THIN, .high = CONNECTOR_THICK, .rot = 1};
		const Asteroid_t CROSS = {.low = QUAD_CORNER, .high = QUAD_CORNER, .rot = 0};
		const Asteroid_t T_EDGE_0 = {.low = T_EDGE_SMALL_CRATER, .high = T_EDGE_BIG_CRATER, .rot = 0};
		const Asteroid_t T_EDGE_1 = {.low = T_EDGE_SMALL_CRATER, .high = T_EDGE_BIG_CRATER, .rot = 1};
		const Asteroid_t T_EDGE_2 = {.low = T_EDGE_SMALL_CRATER, .high = T_EDGE_BIG_CRATER, .rot = 2};
		const Asteroid_t T_EDGE_3 = {.low = T_EDGE_SMALL_CRATER, .high = T_EDGE_BIG_CRATER, .rot = 3};
		const Asteroid_t T_CORNER_0 = {.low = T_CORNER_MID, .high = T_CORNER_HIGH, .rot = 0};
		const Asteroid_t T_CORNER_1 = {.low = T_CORNER_MID, .high = T_CORNER_HIGH, .rot = 1};
		const Asteroid_t T_CORNER_2 = {.low = T_CORNER_MID, .high = T_CORNER_HIGH, .rot = 2};
		const Asteroid_t T_CORNER_3 = {.low = T_CORNER_MID, .high = T_CORNER_HIGH, .rot = 3};
		const Asteroid_t SHALLOW_0 = {.low = SHALLOW, .high = SHALLOW, .rot = 0};
		const Asteroid_t LC_EDGE_0 = {.low = LC_EDGE_SMALL_CRATER, .high = LC_EDGE_BIG_CRATER, .rot = 0};
		const Asteroid_t LC_EDGE_1 = {.low = LC_EDGE_SMALL_CRATER, .high = LC_EDGE_BIG_CRATER, .rot = 1};
		const Asteroid_t LC_EDGE_2 = {.low = LC_EDGE_SMALL_CRATER, .high = LC_EDGE_BIG_CRATER, .rot = 2};
		const Asteroid_t LC_EDGE_3 = {.low = LC_EDGE_SMALL_CRATER, .high = LC_EDGE_BIG_CRATER, .rot = 3};
		const Asteroid_t RC_EDGE_0 = {.low = RC_EDGE_SMALL_CRATER, .high = RC_EDGE_BIG_CRATER, .rot = 0};
		const Asteroid_t RC_EDGE_1 = {.low = RC_EDGE_SMALL_CRATER, .high = RC_EDGE_BIG_CRATER, .rot = 1};
		const Asteroid_t RC_EDGE_2 = {.low = RC_EDGE_SMALL_CRATER, .high = RC_EDGE_BIG_CRATER, .rot = 2};
		const Asteroid_t RC_EDGE_3 = {.low = RC_EDGE_SMALL_CRATER, .high = RC_EDGE_BIG_CRATER, .rot = 3};
		const Asteroid_t TRIPLE_0 = {.low = TRIPLE_CORNER, .high = TRIPLE_CORNER, .rot = 0};
		const Asteroid_t TRIPLE_1 = {.low = TRIPLE_CORNER, .high = TRIPLE_CORNER, .rot = 1};
		const Asteroid_t TRIPLE_2 = {.low = TRIPLE_CORNER, .high = TRIPLE_CORNER, .rot = 2};
		const Asteroid_t TRIPLE_3 = {.low = TRIPLE_CORNER, .high = TRIPLE_CORNER, .rot = 3};
		const Asteroid_t C_CORNER_0 = {.low = CROSS_CORNER, .high = CROSS_CORNER, .rot = 0};
		const Asteroid_t C_CORNER_1 = {.low = CROSS_CORNER, .high = CROSS_CORNER, .rot = 3};

		// referce for the tiles to be used in drawing
		const Asteroid_t* TILE_REF[256] = {
			&LONE_0,
			&LONE_0,
			&TOP_0,
			&TOP_0,
			&LONE_0,
			&LONE_0,
			&TOP_0,
			&TOP_0,
			&TOP_3,
			&TOP_3,
			&T_CORNER_3,
			&CORNER_3,
			&TOP_3,
			&TOP_3,
			&T_CORNER_3,
			&CORNER_3,
			&TOP_1,
			&TOP_1,
			&T_CORNER_0,
			&T_CORNER_0,
			&TOP_1,
			&TOP_1,
			&CORNER_0,
			&CORNER_0,
			&CONNECTOR_1,
			&CONNECTOR_1,
			&T_CORNER_3,
			&LC_EDGE_3,
			&CONNECTOR_1,
			&CONNECTOR_1,
			&RC_EDGE_3,
			&EDGE_3,
			&LONE_0,
			&LONE_0,
			&TOP_0,
			&TOP_0,
			&LONE_0,
			&LONE_0,
			&TOP_0,
			&TOP_0,
			&TOP_3,
			&TOP_3,
			&T_CORNER_3,
			&CORNER_3,
			&TOP_3,
			&TOP_3,
			&T_CORNER_3,
			&CORNER_3,
			&TOP_1,
			&TOP_1,
			&T_CORNER_0,
			&T_CORNER_0,
			&TOP_1,
			&TOP_1,
			&CORNER_0,
			&CORNER_0,
			&CONNECTOR_1,
			&CONNECTOR_1,
			&T_CORNER_3,
			&LC_EDGE_3,
			&CONNECTOR_1,
			&CONNECTOR_1,
			&RC_EDGE_3,
			&EDGE_3,
			&TOP_2,
			&TOP_2,
			&CONNECTOR_0,
			&CONNECTOR_0,
			&TOP_2,
			&TOP_2,
			&CONNECTOR_0,
			&CONNECTOR_0,
			&T_CORNER_2,
			&T_CORNER_2,
			&T_EDGE_2,
			&RC_EDGE_2,
			&T_CORNER_2,
			&T_CORNER_2,
			&T_EDGE_2,
			&RC_EDGE_2,
			&T_CORNER_1,
			&T_CORNER_1,
			&T_EDGE_0,
			&T_EDGE_0,
			&T_CORNER_1,
			&T_CORNER_1,
			&LC_EDGE_0,
			&LC_EDGE_0,
			&T_EDGE_1,
			&T_EDGE_1,
			&CROSS,
			&TRIPLE_0,
			&T_EDGE_1,
			&T_EDGE_1,
			&TRIPLE_1,
			&D_I_CORNER_0,
			&TOP_2,
			&TOP_2,
			&CONNECTOR_0,
			&CONNECTOR_0,
			&TOP_2,
			&TOP_2,
			&CONNECTOR_0,
			&CONNECTOR_0,
			&CORNER_2,
			&CORNER_2,
			&LC_EDGE_2,
			&EDGE_2,
			&CORNER_2,
			&CORNER_2,
			&LC_EDGE_2,
			&EDGE_2,
			&T_CORNER_1,
			&T_CORNER_1,
			&T_EDGE_0,
			&T_EDGE_0,
			&T_CORNER_1,
			&T_CORNER_1,
			&LC_EDGE_0,
			&LC_EDGE_0,
			&RC_EDGE_1,
			&RC_EDGE_1,
			&TRIPLE_3,
			&D_I_CORNER_3,
			&RC_EDGE_1,
			&RC_EDGE_1,
			&C_CORNER_1,
			&I_CORNER_3,
			&LONE_0,
			&LONE_0,
			&TOP_0,
			&TOP_0,
			&LONE_0,
			&LONE_0,
			&TOP_0,
			&TOP_0,
			&TOP_3,
			&TOP_3,
			&T_CORNER_3,
			&CORNER_3,
			&TOP_3,
			&TOP_3,
			&T_CORNER_3,
			&CORNER_3,
			&TOP_1,
			&TOP_1,
			&T_CORNER_0,
			&T_CORNER_0,
			&TOP_1,
			&TOP_1,
			&CORNER_0,
			&CORNER_0,
			&CONNECTOR_1,
			&CONNECTOR_1,
			&T_EDGE_3,
			&LC_EDGE_3,
			&CONNECTOR_1,
			&CONNECTOR_1,
			&RC_EDGE_3,
			&EDGE_3,
			&LONE_0,
			&LONE_0,
			&TOP_0,
			&TOP_0,
			&LONE_0,
			&LONE_0,
			&TOP_0,
			&TOP_0,
			&TOP_3,
			&TOP_3,
			&T_CORNER_3,
			&CORNER_3,
			&TOP_3,
			&TOP_3,
			&T_CORNER_3,
			&CORNER_3,
			&TOP_1,
			&TOP_1,
			&T_CORNER_0,
			&T_CORNER_0,
			&TOP_1,
			&TOP_1,
			&CORNER_0,
			&CORNER_0,
			&CONNECTOR_1,
			&CONNECTOR_1,
			&T_EDGE_3,
			&LC_EDGE_3,
			&CONNECTOR_1,
			&CONNECTOR_1,
			&RC_EDGE_3,
			&EDGE_3,
			&TOP_2,
			&TOP_2,
			&CONNECTOR_0,
			&CONNECTOR_0,
			&TOP_2,
			&TOP_2,
			&CONNECTOR_0,
			&CONNECTOR_0,
			&T_CORNER_2,
			&T_CORNER_2,
			&T_EDGE_2,
			&RC_EDGE_2,
			&T_CORNER_2,
			&T_CORNER_2,
			&T_EDGE_2,
			&RC_EDGE_2,
			&CORNER_1,
			&CORNER_1,
			&RC_EDGE_0,
			&RC_EDGE_0,
			&CORNER_1,
			&CORNER_1,
			&EDGE_0,
			&EDGE_0,
			&LC_EDGE_1,
			&LC_EDGE_1,
			&TRIPLE_2,
			&C_CORNER_0,
			&LC_EDGE_1,
			&LC_EDGE_1,
			&D_I_CORNER_1,
			&I_CORNER_0,
			&TOP_2,
			&TOP_2,
			&CONNECTOR_0,
			&CONNECTOR_0,
			&TOP_2,
			&TOP_2,
			&CONNECTOR_0,
			&CONNECTOR_0,
			&CORNER_2,
			&CORNER_2,
			&LC_EDGE_2,
			&EDGE_2,
			&CORNER_2,
			&CORNER_2,
			&LC_EDGE_2,
			&EDGE_2,
			&CORNER_1,
			&CORNER_1,
			&RC_EDGE_1,
			&RC_EDGE_1,
			&CORNER_1,
			&CORNER_1,
			&EDGE_0,
			&EDGE_0,
			&EDGE_1,
			&EDGE_1,
			&D_I_CORNER_2,
			&I_CORNER_2,
			&EDGE_1,
			&EDGE_1,
			&I_CORNER_1,
			&SHALLOW_0
		};

		// map the depths of the asteroid
		void map_depths();

		// takes the asteroid after it has been depth mapped, and maps tiles to it
		// rotations are stored in the array in the 16 bit position (r = num/65536)
		void map_tiles();

};

#endif
