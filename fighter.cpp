#include "fighter.h"


// base constructor
Fighter::Fighter(SDL_Renderer* gRenderer, int screen_w, int screen_h, int p_x, int p_y,  Frame_Counter* timer, std::queue<Entity*>* e_list, std::queue<Projectile*>* p_list, std::queue<Particle*>* pt_list)
       	: Entity{"Tiles/fighter_tiles.png","", gRenderer,e_list,p_list,2,2} {

	// set our position
	m_p_x = p_x;
	m_p_y = p_y;
	m_v_x = 0;
	m_v_y = 0;

	// set our type
	m_type = FIGHTER;

	// set the screen
	m_screen_w = screen_w;
	m_screen_h = screen_h;
	
	// set our timer, and particles list
	m_timer = timer;
	m_pt_list = pt_list;

	// const radius for a 2 tile craft
	m_radius = 23;

	// set our team
	m_team = 24;

	// set renderer
	m_renderer = gRenderer;

	// init health and damage state
	m_health = m_max_health;
	m_damage_state = 0;
	m_boost = m_max_boost;
	m_recharge = false;

	// set our fighter array
	m_array[0][0].indx = F_FRONT_L;
	m_array[0][1].indx = F_FRONT_R;
	m_array[1][0].indx = F_BACK_L;
	m_array[1][1].indx = F_BACK_R;	
	
}

void Fighter::render_HUD(){

	// find the rectagles for the health and boost
	SDL_Rect health_bar = {10,10,24,static_cast<int>(100.0 * (static_cast<double>(m_health) / static_cast<double>(m_max_health)))};
	SDL_Rect h_bar_outline = {9,9,26,102};
	SDL_Rect boost_bar = {36,10,24,static_cast<int>(100.0 * (static_cast<double>(m_boost) / static_cast<double>(m_max_boost)))};
	SDL_Rect b_bar_outline = {35,9,26,102}; 

	// draw them to screen
	SDL_SetRenderDrawColor(m_renderer,0xff,0x00,0x00,0xff);
	SDL_RenderFillRect(m_renderer, &health_bar);
	SDL_SetRenderDrawColor(m_renderer,0xff,0xD8,0x00,0xff);
	SDL_RenderFillRect(m_renderer, &boost_bar);
	SDL_SetRenderDrawColor(m_renderer,0x95,0x95,0x95,0xff);
	SDL_RenderDrawRect(m_renderer, &h_bar_outline);
	SDL_RenderDrawRect(m_renderer, &b_bar_outline);

	// inverse square root
	double def = 1 / sqrt(m_p_x * m_p_x + m_p_y * m_p_y);

	SDL_RenderDrawLine(m_renderer, (int) (-m_p_x * 30 * def) + m_screen_w / 2, (int) (-m_p_y * 30 * def) + m_screen_h / 2, (int) (-m_p_x * 60 * def) + m_screen_w / 2,(int)  (-m_p_y * 60 * def) + m_screen_h / 2);
	
	SDL_SetRenderDrawColor(m_renderer,0x00,0x00,0x00,0xff);

}

// upon death of the fighter, add some particles
void Fighter::death_particles(){
	
	Particle* p;

	// add metal beam scrap
	for (int i = 0; i < 2; i++){
		p = Particle_Engine::p_alloc(1);
		p->frames[0] = (rand() % 3) + 8;
		p->sheet = PE_GENERAL;
		p->p_x = m_p_x + rand_double(10.0,-10.0);
		p->p_y = m_p_y + rand_double(10.0,-10.0);
		p->rot = rand_double(360.0,0.0);
		p->v_x = rand_double(3.0,-3.0);
		p->v_y = rand_double(3.0,-3.0);
		p->v_r = rand_double(10,-10);
		p->lifetime = 900 + (rand() % 256);
		m_pt_list->push(p);
        }

	// add metal plate scrap
	for (int i = 0; i < 2; i++){
		p = Particle_Engine::p_alloc(1);
		p->frames[0] = (rand() % 3) + 28;
		p->sheet = PE_GENERAL;
		p->p_x = m_p_x + rand_double(10.0,-10.0);
		p->p_y = m_p_y + rand_double(10.0,-10.0);
		p->rot = rand_double(360.0,0.0);
		p->v_x = rand_double(3.0,-3.0);
		p->v_y = rand_double(3.0,-3.0);
		p->v_r = rand_double(10,-10);
		p->lifetime = 900 + (rand() % 256);
		m_pt_list->push(p);
        }

	// add fire particles
	int num = 10 + (rand() % 10);
	for (int i = 0; i < num; i++){
		p = Particle_Engine::p_alloc(4);
		p->frames[0] = 4;
		p->frames[1] = 5;
		p->frames[2] = 6;
		p->frames[3] = 7;
		p->sheet = PE_GENERAL;
		p->p_x = m_p_x + rand_double(10.0,-10.0);
		p->p_y = m_p_y + rand_double(10.0,-10.0);
		p->rot = rand_double(360.0,0.0);
		p->v_x = rand_double(3.0,-3.0);
		p->v_y = rand_double(3.0,-3.0);
		p->v_r = rand_double(10,-10);
		p->lifetime = 800 + (rand() % 256);
		m_pt_list->push(p);
        }
}

// collide this entity with damage
int Fighter::collide(double x, double y, int damage){

	if (m_dead){
		return 0;	
	}

	// find the sine and cosine of point from us
	//
	//  |   /
	//  |  /
	//  | /  ) <-- Here
	//  |/_______
	double d_x = x - m_p_x;
	double d_y = y - m_p_y;
	double hyp = sqrt(d_x * d_x + d_y * d_y);
	double c_sin = -d_y / hyp;
	double c_cos = d_x / hyp;
	
	// update our velocity
	double d_v = sqrt(static_cast<double>(damage * 2) / static_cast<double>(m_mass));
	m_v_x -= d_v * c_cos;
	m_v_y -= d_v * c_sin;

	// update our health
	int temp_h = m_health;

	if (damage >= m_health){
		m_health = 0;
		m_dead = true;
		death_particles();
	} else {
		m_health -= damage;
	}

	return temp_h;

}

void Fighter::forward(Uint8 boost){
	
	double rad_rot = m_rot * RAD_CONST;

	if (rand() % 10 > 5) {
		add_engine_particle(4,15,m_rot);
	}
	if (rand() % 10 > 5) {
		add_engine_particle(-4,15,m_rot);
	}

	// boost with num key
	if (boost && ((m_boost > 0 && !m_recharge) || m_boost > m_boost_recharge)){
		m_v_x += 4 * sin(rad_rot);
               	m_v_y += 4 * cos(rad_rot);
		m_array[1][0].indx = F_BACK_BOOST_L + m_damage_state * 20;
		m_array[1][1].indx = F_BACK_BOOST_R + m_damage_state * 20;
		
		m_boost -= 2;
		if (m_boost == 0){
			m_recharge = true;
		} 

		if (rand() % 10 > 5) {
			add_engine_particle(4,15,m_rot);
		}
		if (rand() % 10 > 5) {
			add_engine_particle(-4,15,m_rot);
		}
	} else {
		m_v_x += 1.5 * sin(rad_rot);
		m_v_y += 1.5 * cos(rad_rot);
                m_array[1][0].indx = F_BACK_ON_L + m_damage_state * 20;
               	m_array[1][1].indx = F_BACK_ON_R + m_damage_state * 20;

	}

}

// do what needs to be done when keys are pressed
void Fighter::check_keys(const Uint8* key_states){
	
	double rad_rot;

	// turn right
	if (key_states[SDL_SCANCODE_RIGHT]){
		m_rot += 5;
        }

	// turn left
        if (key_states[SDL_SCANCODE_LEFT]){
                m_rot -= 5;
        }
        
	// go forward
	if (key_states[SDL_SCANCODE_UP]){

		forward(key_states[SDL_SCANCODE_RCTRL]);

        } else {

               	m_array[1][0].indx = F_BACK_L + m_damage_state * 20;
                m_array[1][1].indx = F_BACK_R + m_damage_state * 20;
        }
	
	// do work with the boosters	
	if (!key_states[SDL_SCANCODE_RCTRL]){
		m_boost += 1 + (!m_recharge);
	}

	if (m_boost > m_max_boost){
		m_boost = m_max_boost;
	}

	if (m_boost > m_boost_recharge) {
		m_recharge = false;
	}
        
	// brake
	if (key_states[SDL_SCANCODE_DOWN]){
                rad_rot = m_rot * RAD_CONST;
                m_v_x -= sin(rad_rot);
                m_v_y -= cos(rad_rot);
        }


	// shoot
	if (key_states[SDL_SCANCODE_F]){
		// check if we are cooled down
		if (m_timer->check_timer(m_last_shot) > m_cooldown){
			fire_weapons();
		}
	}


}

// check for bounds, zero things out, etc
void Fighter::bounds_check(){

	// check rotation to prevent overflow
        if (m_rot < 0.0){
                m_rot += 360.0;
        }
        if (m_rot > 360.0){
                m_rot -= 360.0;
        }

        // if velocity if low enough, make it zero
        if (std::abs(m_v_x) < 0.01f) {
                m_v_x = 0.0;
        }
        if (std::abs(m_v_y) < 0.01f){
                m_v_y = 0.0;
        }

	/*
	// do bound detection
        if (m_p_y > m_screen_h){
                m_p_y -= m_screen_h;
        }
	if (m_p_y < 0){
                m_p_y += m_screen_h;
        }
        if (m_p_x > m_screen_w){
                m_p_x -= m_screen_w;
        }
        if (m_p_x < 0){
                m_p_x += m_screen_w;
        }
	*/
}

// modify position, velocity, etc
void Fighter::update(){

	if (m_dead){
		return;
	}

        // apply friction
       	m_v_x *= 0.9f;
        m_v_y *= 0.9f;

	// update our position
        m_p_y -= m_v_y;
        m_p_x += m_v_x;

	update_trig();

	check_collisions();

	int num = rand() % 20;

	if (m_health <= (m_max_health * 2) / 3){
		m_damage_state = 1;

		if (num == 0){
			add_damage_particle(10,-1);
		} else if (num == 1){
			add_damage_particle(-15,1);
		} else if (num == 2){
			add_damage_particle(-3,12);
		}

	}

	if (m_health <= m_max_health / 3){
		m_damage_state = 2;
		if (num == 0){
			add_damage_particle(-5,-5);
		} else if (num == 1){
			add_damage_particle(-2,-12);
		} else if (num == 2){
			add_damage_particle(3,12);
		}

	}

	if (m_health <= 0){
		m_dead = true;
		m_health = 0;
		return;
	}

	// reset gun graphics
	if (m_timer->check_timer(m_last_shot) > m_sprite_cooldown){
		        m_array[0][0].indx = F_FRONT_L + m_damage_state * 20;
		       	m_array[0][1].indx = F_FRONT_R + m_damage_state * 20;
	}

	// check bounds
	bounds_check();

}

void Fighter::draw(double tc_x, double tc_y){

	m_vis.render(static_cast<int>(m_p_x - tc_x), static_cast<int>(m_p_y - tc_y), m_rot);

	render_HUD();
}

void Fighter::add_engine_particle(int x, int y, double rot){
       	
        double sin_rad = sin(rot * RAD_CONST);
	double cos_rad = cos(rot * RAD_CONST);

	Particle* p = Particle_Engine::p_alloc(4);
	p->frames[0] = 4;
	p->frames[1] = 5;
	p->frames[2] = 6;
	p->frames[3] = 7;

	p->sheet = PE_GENERAL;
	p->p_x = m_p_x - (y * sin_rad) - (cos_rad * x);
	p->p_y = m_p_y + (y * cos_rad) - (sin_rad * x);
	p->rot = rand_double(360.0,0.0);
	p->v_x = -rand_double(9.0,8.0) * sin((rot + rand_double(4.0,-4.0)) * RAD_CONST);
	p->v_y = rand_double(9.0,8.0) * cos((rot + rand_double(4.0,-4.0)) * RAD_CONST);
	p->v_r = rand_double(10.0,-10.0);
	p->lifetime = Uint32(500 + (rand() % 50));
	m_pt_list->push(p);

}

// add damage particle
void Fighter::add_damage_particle(int x, int y){
        
	double sin_rad = sin(m_rot * RAD_CONST);
	double cos_rad = cos(m_rot * RAD_CONST);

	Particle* p = Particle_Engine::p_alloc(3);
	p->frames[0] = 5;
	p->frames[1] = 6;
	p->frames[2] = 7; 

	p->sheet = PE_GENERAL;
	p->p_x = m_p_x - (y * sin_rad) - (cos_rad * x);
	p->p_y = m_p_y + (y * cos_rad) + (sin_rad * (-x));
	p->rot = rand_double(360.0,0.0);
	p->v_x = rand_double(1.5,-1.5);
	p->v_y = rand_double(1.5,-1.5);
	p->v_r = rand_double(10.0,-10.0);
	p->lifetime = Uint32(200 + (rand() % 150));
	m_pt_list->push(p);

}


void Fighter::fire_weapons(){

	// set our sprite
	m_array[0][0].indx = F_FRONT_FIRING_L + m_damage_state * 20;
	m_array[0][1].indx = F_FRONT_FIRING_R + m_damage_state * 20;

	double rad_rot = m_rot * RAD_CONST;
	double sin_rad = sin(rad_rot);
	double cos_rad = cos(rad_rot);

	m_last_shot = m_timer->start_timer();

	// allocate a particle and projectile
	Projectile* p = Particle_Engine::pr_alloc(1);

	// set particle stuff
	p->p->sheet = PE_GENERAL;
	p->p->frames[0] = 11;
	p->p->p_x = m_p_x - (-11 * sin_rad) - (cos_rad * 14);
	p->p->p_y = m_p_y + (-11 * cos_rad) + (sin_rad * -14);
	p->p->rot = m_rot;
	p->p->v_x = m_v_x + (10.0 * sin_rad);
	p->p->v_y = (-10.0 * cos_rad) - m_v_y;
	p->p->v_r = 0;
	p->p->lifetime = Uint32(650);

	// set projectile stuff
	p->damage = 5;
	p->team = m_team;
	p->radius = DEFAULT_RADIUS;

	// put it in list
	m_p_list->push(p);

	// allocate a particle and projectile
	p = Particle_Engine::pr_alloc(1);

	// set particle stuff
	p->p->sheet = PE_GENERAL;
	p->p->frames[0] = 11;
	p->p->p_x = m_p_x - (-11 * sin_rad) - (cos_rad * -14);
	p->p->p_y = m_p_y + (-11 * cos_rad) + (sin_rad * 14);
	p->p->rot = m_rot;
	p->p->v_x = m_v_x + (10.0 * sin_rad);
	p->p->v_y = (-10.0 * cos_rad) - m_v_y;
	p->p->v_r = 0;
	p->p->lifetime = Uint32(650);

	// set projectile stuff
	p->damage = 5;
	p->team = m_team;
	p->radius = DEFAULT_RADIUS;

	// put it in list
	m_p_list->push(p);

}
