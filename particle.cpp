#include "particle.h"

// constructor for patricle engine
Particle_Engine::Particle_Engine(SDL_Renderer* gRenderer, std::vector<std::string> file_list, Frame_Counter* counter){
	m_counter = counter;
	m_vis = {};
	Tile** temp;
	SDL_Point c = {12,12};
	for (int i = 0; i < static_cast<int>(file_list.size()); i++){
		
		// put a new sprite on list
		m_vis.push_back(new Sprite());
		m_vis[i]->set_sprite_sheet("Particles/" + file_list[i],"",gRenderer);
		
		// make an array for sprite
		temp = (Tile**) malloc(sizeof(Tile*));
		temp[0] = (Tile*) malloc(sizeof(Tile));
		temp[0][0] = {0,0,0,0};

		// give array to sprite
		m_arrays.push_back(temp);
		m_vis[i]->set_array(temp,1,1);
		m_vis[i]->set_center(c);

	}
	
}

// destructor
Particle_Engine::~Particle_Engine(){

	// delete particle list
	for (int i = 0; i < static_cast<int>(m_vis.size()); i++){
		delete m_vis[i];
	}

}

// update the given particle
void Particle_Engine::update_particle(Particle* p){

	// get time
	Uint32 t = m_counter->check_timer(p->start_time);

	// check if dead
	if (t >= p->lifetime){
		p->is_dead = true;
		return;
	}
	
	// update frame
	p->c_frame = p->frames[static_cast<int>((static_cast<double>(p->frames_len) * static_cast<double>(t)) / static_cast<double>(p->lifetime))];

	// update position and rotation
	p->p_x += p->v_x;
	p->p_y += p->v_y;
	p->rot += p->v_r;

}

// adjust array, and draw to screen
void Particle_Engine::draw_particle(const Particle* p, double tc_x, double tc_y){

	// put new value in array
	m_arrays[p->sheet][0][0].indx = p->c_frame;
	// render particle
	m_vis[p->sheet]->render(static_cast<int>(p->p_x - tc_x),static_cast<int>(p->p_y - tc_y),p->rot);
	

}

// delete given particle
void Particle_Engine::free_p(Particle* p){
	free(p->frames);
	free(p);
}

// delete given projectile
void Particle_Engine::free_pr(Projectile* p){

	free_p(p->p);

	// free subparticle
	if (p->sub_p != nullptr){
		free_p(p->sub_p);
	}

	free(p);

}


// allocate a particle with the given frame array size
Particle* Particle_Engine::p_alloc(int size){
	short int* p_list = static_cast<short int*>(calloc(size,sizeof(short int)));
	Particle* p = static_cast<Particle*>(calloc(1,sizeof(Particle)));
	p->frames = p_list;
	p->frames_len = size;
	p->c_frame = 1;
	p->start_time = SDL_GetTicks();
	p->is_dead = false;
	return p;
}

// allocate a projectile with given number of frames
Projectile* Particle_Engine::pr_alloc(int size){
	Projectile* p = static_cast<Projectile*>(calloc(1,sizeof(Projectile)));
	p->p = p_alloc(size);
	p->sub_particles = false;
	p->sub_percent = 0;
	p->sub_p = nullptr;
	p->explode = false;
	p->e_radius = 0;
	p->e_damage = 0;
	return p;
}
