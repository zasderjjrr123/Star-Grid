#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#ifndef SPRITE_H
#define SPRITE_H

#define SPRITE_SIZE 24
#define SPRITE_SHEET_WIDTH 20
#define SPRITE_SHEET_HEIGHT 20

// tile that is used in rendering / gameplay
typedef struct tile {
	
	// index into the sprite sheet
	int indx;
	// rotation (0|1|2|3 * 90)
	int rotation;
	// current health of module
	int health;
	// weight of tile
	int weight;
	// index of tile in the referce list
	int ref_indx;
	// damage state
	int damage_state;

} Tile;

// sprite for actuallt drawing on the screen
class Sprite {

	public:
		Sprite();
		~Sprite();

		// set the sprite sheet for tiles and damage (NULL if no damage)
		bool set_sprite_sheet(std::string path, std::string d_path, SDL_Renderer* gRenderer);
		
		// give an array pointer and width and height of array, as well as center
		void set_array(Tile** array, int width, int height);
	
		// set the center of the sprite to position and rotate around
		void set_center(SDL_Point center);

		// delete stuff
		void s_free();
		
		// render given entity with center at x, y from top left, and rotations
		void render(int x, int y, double rot);
	
		// get the texture pointer
		SDL_Texture* get_texture(){return m_texture;}

	private:

		// array, width, and height
		int m_width;
		int m_height;
		Tile** m_array;
		
		// center (in pixels from top left)
		SDL_Point m_center;

		// do we have tile damage?
		bool m_damage;

		// texture for tiles
		SDL_Texture* m_texture;
		
		// texture for damage
		SDL_Texture* m_d_texture;
		
		// renderer
		SDL_Renderer* m_renderer;

};

#endif
