#include "sprite.h"
#include <stdlib.h>

// constructor
Sprite::Sprite(){
	m_texture = NULL;
	m_d_texture = NULL;
	m_array = nullptr;
	m_height = 0;
	m_width = 0;
	m_center = {12,12};
	m_damage = false;
}

// destructor
Sprite::~Sprite(){s_free();}

// free this texture
void Sprite::s_free(){
	if (m_texture != NULL){
		SDL_DestroyTexture(m_texture);
		m_texture = NULL;
	}
	if (m_d_texture != NULL){
		SDL_DestroyTexture(m_texture);
		m_texture = NULL;
	}

	for (int i = 0; i < m_width; i++){
                free(m_array[i]);
        }

	free(m_array);

	m_array = nullptr;
	m_width = 0;
	m_height = 0;
}

// set the sheet form which we will load our sprites
bool Sprite::set_sprite_sheet(std::string path, std::string d_path, SDL_Renderer* gRenderer){

	s_free();
	
	m_renderer = gRenderer;

	SDL_Surface* loaded = NULL;
	SDL_Texture* new_t = NULL;

	// try to load image, return if failed
	loaded = IMG_Load(path.c_str());
	if (loaded == NULL){
		printf("Error loading image %s: %s", path.c_str(), SDL_GetError());
		return false;
	}

	// recolor surface
	SDL_SetColorKey(loaded, SDL_TRUE, SDL_MapRGB(loaded->format, 0,0,0));

	new_t = SDL_CreateTextureFromSurface(m_renderer,loaded);
	if (new_t == NULL){
		printf("Error creating texture: %s", SDL_GetError());
		return false;
	}

	m_texture = new_t;
	SDL_FreeSurface(loaded);

	if (d_path != "") {

		m_damage = true;

		// try to load image, return if failed
		loaded = IMG_Load(d_path.c_str());
		if (loaded == NULL){
			printf("Error loading image %s: %s", d_path.c_str(), SDL_GetError());
			return false;
		}

		// recolor surface
		SDL_SetColorKey(loaded, SDL_TRUE, SDL_MapRGB(loaded->format, 0xFF,0xFF,0xFF));

		new_t = SDL_CreateTextureFromSurface(m_renderer,loaded);
		if (new_t == NULL){
			printf("Error creating texture: %s", SDL_GetError());
			return false;
		}

		m_d_texture = new_t;
		SDL_FreeSurface(loaded);
	}

	return true;
	
}

// set the array (with width and height)
void Sprite::set_array(Tile** array, int width, int height){
	m_array = array;
	m_width = width;
	m_height = height;
}

// set the center of the sprite, for position and rotation
void Sprite::set_center(SDL_Point center) {

	m_center.x = center.x;
	m_center.y = center.y;
}

// render this sprite on screen with center at x and y, and given rotation
void Sprite::render(int x, int y, double rot){

	// make sure there is a sprite to render
	if (m_array == nullptr){
		return;
	}

	// find the middle of the shape, to rotate around it
	int med_w = m_center.y;
	int med_h = m_center.x;

	// set the center so it rotates around there
	x -= med_w;
	y -= med_h;

	// set rotations rot tiles
	const int ROT_SIZE = SPRITE_SIZE * 5;
	const int ROT_SHEET[4] = {0, ROT_SIZE, ROT_SIZE * 2, ROT_SIZE * 3};

	// declare our num and direction
	int num, dir;

	// rectangle where we are getting tile
	SDL_Rect src;
	src.h = SPRITE_SIZE;
	src.w = SPRITE_SIZE;

	// rectangle where we are putting tile
	SDL_Rect dst;
	dst.h = SPRITE_SIZE;
	dst.w = SPRITE_SIZE;

	// center point
	SDL_Point c;
	
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			
			// get tile number
			num = m_array[j][i].indx;

			// if there is no tile, dont render	
			if (!num){
				continue;
			}

			// get direction
			dir = m_array[j][i].rotation;
			
			// center it
			c = {(med_w - i * SPRITE_SIZE), (med_h - j * SPRITE_SIZE)};

			// get our image from sprite sheet
			src.x = (num % SPRITE_SHEET_WIDTH) * SPRITE_SIZE;
			src.y = (num / SPRITE_SHEET_WIDTH) * SPRITE_SIZE + ROT_SHEET[dir];
			
			// put it on the screen
			dst.x = (i * SPRITE_SIZE) + x;
			dst.y = (j * SPRITE_SIZE) + y;

			SDL_RenderCopyEx(m_renderer, m_texture, &src, &dst, rot, &c, SDL_FLIP_NONE);
			
			// check for damage
			if (m_damage){
				// get our damage state
				num = m_array[j][i].damage_state;

				// update sprite with it
				src.x = (num % SPRITE_SHEET_WIDTH) * SPRITE_SIZE;
				src.y = (num / SPRITE_SHEET_WIDTH) * SPRITE_SIZE;
				SDL_RenderCopyEx(m_renderer, m_d_texture, &src, &dst, rot, &c, SDL_FLIP_NONE);
			}
		}
	}
}
