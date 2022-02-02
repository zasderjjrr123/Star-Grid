#include "asteroid.h"

// default constructor for asteroid
Asteroid::Asteroid(SDL_Renderer* gRenderer, std::queue<Entity*>* e_list,  std::queue<Particle*>* pt_list, std::queue<Projectile*>* pr_list, double p_x, double p_y, double rot, double rot_rate, int width, int height)
       	: Entity {"Tiles/asteroid_tiles.png","Tiles/damage_tiles.png", gRenderer,e_list,pr_list,height,width} {

		// set position
		m_p_x = p_x;
		m_p_y = p_y;
		m_rot = rot;
		m_v_r = rot_rate;
		m_type = ASTEROID;
		
		// set our particle and projectile list
		m_pt_list = pt_list;

		// set array
		set_asteroid();
		m_center = center_of_mass();
		m_vis.set_center(m_center);

		// find the radius of our asteroid
		double width_px = m_width * 12;
		if (m_center.x > width_px){
			width_px += std::abs(m_center.x - width_px);
		} else {
			width_px += std::abs(width_px - m_center.x);
		}
		double height_px = m_height * 12;
		if (m_center.y > height_px){
			height_px += std::abs(m_center.y - height_px);
		} else {
			height_px += std::abs(height_px - m_center.y);
		}
		
		m_radius = sqrt(width_px * width_px + height_px * height_px);

}

// for when an entity is colliding with us
int Asteroid::collide(double d_x, double d_y, int damage){
	
	if (m_dead){
		return 0;
	}

	int i, j;
	int h;

	rotate_g_to_e(d_x,d_y,i,j);

	if (i >= 0 && i < m_width && j >= 0 && j < m_height && m_array[i][j].indx){
		h = m_array[i][j].health;
		damage_module(i,j,damage,d_x,d_y);
		return h;
	} else {
		return 0;
	}
}

// collide with the asteroid at x and y
void Asteroid::damage_module(int x, int y, int damage, double d_i, double d_j){
	
	// if no tile, do nothing
	if (m_array[x][y].indx == 0){
		return;
	}

	// if module survived
	if (m_array[x][y].health > damage) {

		// subtract health
		m_array[x][y].health -= damage;

		// update damage state sprite
		if (m_array[x][y].indx == 1) {
			m_array[x][y].damage_state = (3 * (m_heavy_health - m_array[x][y].health - 1)) / m_heavy_health;
		} else {
			m_array[x][y].damage_state = (3 * (m_light_health - m_array[x][y].health - 1)) / m_light_health;
		}

	// module did not survive
	} else {
		
		add_debris(d_i,d_j,m_array[x][y].indx == 1);

		// first, delete block
		m_array[x][y] = Tile{0,0,0,0,0,0};
	
		m_total -= 1;

		int indx = 0, roll = 0;
		// then update 8 around it
		for (int i = -1; i <= 1; i++){
			for (int j = -1; j <= 1; j++){
				if (x + i < m_width && x + i >= 0 && y + j < m_height && y + j > 0 && m_array[x+i][y+j].indx != 0 && m_array[x+i][y+j].indx != 1) {
					indx = 0;
					roll = 1;
					for (int k = -1; k <= 1; k++) {
						for (int l = -1; l <= 1 && l >=-1; l++) {
							if (k || l){
								if ((x+i)+k >= 0 && (x+i)+k < m_width && (y+j)+l >= 0 && (y+j)+l < m_height){
									if (m_array[(x+i)+k][(y+j)+l].indx){
										indx += roll;
									}	
								}
								roll *= 2;
							}
						}
					}
			
					if (indx == 255){	
						m_array[x+i][y+j].indx = 2;
						m_array[x+i][y+j].rotation = get_rand_int(0,3);
					} else if (indx == 0){
						m_array[x+i][y+j].indx = 14;
						m_array[x+i][y+j].rotation = get_rand_int(0,3);	
					} else {
						m_array[x+i][y+j].indx = get_rand_int(TILE_REF[indx]->low,TILE_REF[indx]->high);
						m_array[x+i][y+j].rotation = TILE_REF[indx]->rot;	
					}

				}
			}
		}
	}
}

// add some debris at position x, y
void Asteroid::add_debris(double x, double y, bool heavy) {

	int num = 3 + (rand() % 4);
	Particle* p;
	for (int i = 0; i < num; i++){
		p = Particle_Engine::p_alloc(1);
		p->frames[0] = (rand() % 4) + (20 * heavy);
		p->sheet = PE_GENERAL;
		p->p_x = x + rand_double(5.0,-5.0);
		p->p_y = y + rand_double(5.0,-5.0);
		p->rot = rand_double(360.0,0.0);
		p->v_x = rand_double(3.0,-3.0);
		p->v_y = rand_double(3.0,-3.0);
		p->v_r = rand_double(10,-10);
		p->lifetime = 300 + (rand() % 256);
		m_pt_list->push(p);
	}
}

// perform the update of our asteroid
void Asteroid::update(){

	if (m_dead){
		return;
	}

	// update rotation, then collisions 
	m_rot += m_v_r;
	
	update_trig();

	check_collisions();

}

// draw our asteriod to the screen
void Asteroid::draw(double tc_x, double tc_y){
	m_vis.render(static_cast<int>(m_p_x - tc_x), static_cast<int>(m_p_y - tc_y), m_rot);
}

// gets a random int
int Asteroid::get_rand_int(int min, int max){
	return min + ( rand() % ( max - min + 1) );
}

// gets a random point in outer half of circle
void Asteroid::get_rand_outer_half(int x, int y, int r, int &x2, int &y2){
	int in_r = (r / 2) * (r / 2);
	int out_r = r * r;
	int dx, dy, dr;
	while (true){
		x2 = get_rand_int(x-r,x+r);
		y2 = get_rand_int(y-r,y+r);
		dx = x - x2;
		dy = y - y2;
		dr = (dx * dx) + (dy * dy);
		// if we have a working pair, return it, otherwise, generate another
		if (dr > in_r && dr < out_r){
			return;
		}
	}

}

// set the array of tiles
void Asteroid::set_asteroid(){

	// get tile array
	get_asteroid_shape();

	// map depths
	map_depths();

	// map tiles to rotation and sprite sheet
	map_tiles();

}

// set m_array to 1s for not empty
void Asteroid::get_asteroid_shape(){

	int x, y, r;

	// if asteroid is small, just make it all 1's
	if (m_height < 6 || m_width < 6){
		for (int i = 0; i < m_width; i++){
			for (int j = 0; j < m_height; j++){
				m_array[i][j].indx = 1;
			}
		}
	}
	// if it is a little bigger, do 1 circle
	else if (m_height < 16 || m_width < 16){
		x = m_height / 2;
		y = m_width / 2;
		if ( m_height > m_width ) {
			r = get_rand_int(m_width / 4, m_width / 2);
		} else {
			r = get_rand_int(m_height / 4, m_height / 2);
		}
		put_circle(x, y, r);
	}
	// if its big enough, clump a bunch of circles together
	else {
		int num_clumps = get_rand_int(4,7);
		
		x = m_height / 2;
		y = m_width / 2;
		
		if ( m_height > m_width ) {
			r = get_rand_int(m_width / 4, (3 * m_width) / 8);
		} else {
			r = get_rand_int(m_height / 4, (3 * m_height) / 8);
		}

		put_circle(x, y, r);
		
		int x2, y2, r2;

		for (int i = 1; i < num_clumps - 1; i++){
			get_rand_outer_half(x, y, r, x2, y2);
			r2 = get_rand_int(r / 2, r);
			put_circle(x2, y2, r2);	
		}
	}

}

// put a circle in the array
void Asteroid::put_circle(int x, int y, int r){
	
	// for basic pythagorean theorem
	int check = r * r;
	int xh, yh;
	for (int i = x - r; i < x + r; i++){

		if (i > 0 && i < m_width){

			for (int j = y - r; j < y + r; j++){

				if (j > 0 && j < m_height){

					xh = x - i;
					yh = y - j;
					if ((xh * xh) + (yh * yh) <= check){
						m_array[i][j].indx = 1;
					}
				}
			}
		}
	}
}

// brute force algorith the map depths (4*n^2)
void Asteroid::map_depths(){
	for (int k = 2; k < 5; k++){
		for (int i = 0; i < m_width; i++){
			for (int j = 0; j < m_height; j++){
				if (m_array[i][j].indx){
					if (i > 0){
						if (i < m_width - 1){
							if (j > 0){
								if (j < m_height - 1){
									for (int l = -1; l <= 1; l++){
										for (int m = -1; m <= 1; m++){
											if (l || m) {
												if (k == 2 && m_array[i+l][j+m].indx == 0) {
													m_array[i][j].indx = k;
												} else if(k > 2 && m_array[i+l][j+m].indx == (k-1)){
													m_array[i][j].indx = k;
												}
											}	
										}
									}
								} else {
									m_array[i][j].indx = k;
								}
							} else {
								m_array[i][j].indx = k;
							}
						} else {
							m_array[i][j].indx = k;
						}
						
					} else {
						m_array[i][j].indx = k;
					}
				}	
			}
		}
	}
	
	for (int i = 0; i < m_width; i++){
		for (int j = 0; j < m_height; j++){
			if (m_array[i][j].indx == 2 || m_array[i][j].indx == 3 || m_array[i][j].indx == 4) {
				m_array[i][j].indx = 1;
			} else if (m_array[i][j].indx == 1){
				m_array[i][j].indx = 4;
			}
		}
	}
}

// maps tiles to the asteroid with rotation (see .h file for info)
void Asteroid::map_tiles(){
	int indx, roll;
	for ( int i = 0; i < m_width; i++) {
		for ( int j = 0; j < m_height; j++) {
			if (m_array[i][j].indx == 1) {
				indx = 0;
				roll = 1;
				for (int k = -1; k <= 1; k++) {
					for (int l = -1; l <= 1 && l >=-1; l++) {
						if (k || l){
							if (i+k >= 0 && i+k < m_width && j+l >= 0 && j+l < m_height){
								if (m_array[i+k][j+l].indx){
									indx += roll;
								}
							}
							roll *= 2;
						}
						
					}
				}
			
				if (indx == 255){	
					m_array[i][j].indx = 2;
					m_array[i][j].rotation = get_rand_int(0,3);
				} else if (indx == 0){
					m_array[i][j].indx = 14;
					m_array[i][j].rotation = get_rand_int(0,3);	
				} else {
					m_array[i][j].indx = get_rand_int(TILE_REF[indx]->low,TILE_REF[indx]->high);
					m_array[i][j].rotation = TILE_REF[indx]->rot;	
				}

				m_array[i][j].health = m_light_health;
				m_array[i][j].weight = m_light_weight;
				m_array[i][j].damage_state = 0;
			} else if (m_array[i][j].indx == 4){
				m_array[i][j].indx = 1;
				m_array[i][j].rotation = get_rand_int(0,3);
				m_array[i][j].health = m_heavy_health;
				m_array[i][j].weight = m_heavy_weight;
				m_array[i][j].damage_state = 0;
			}
		}
	}
}
