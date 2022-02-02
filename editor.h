#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "entity.h"
#include "label.h"

#ifndef EDITOR_H
#define EDITOR_H

class Editor : public Entity {

	public:

		// what pallete you want to choose, the height of this ship, and the width of this ship
		Editor(std::string file, SDL_Renderer* gRenderer, Frame_Counter* timer, int height, int width);
		// deconstructor
		~Editor();

		// do actions with the current state of keys
		void check_keys(const Uint8* key_states);
		// update center of mass and piping
		void update();
		// draw the editor on the screen
		void draw(double x, double y);

		// output the ship to file
		void file_export();
		
		// input ship to editor
		void file_import();

	private:

		// the global renderer
		SDL_Renderer* m_renderer;

		// our pointer and pointer array
		Sprite m_pointer;
		Tile** m_p_array;

		// palette and palette size
		std::vector<Tile_Desc> m_palette;
		int m_palette_size;
		
		// was the r key pressed
		bool m_r_pressed;

		Frame_Counter* m_timer;

		// bounds of movement for ship grid
		const SDL_Point m_SG_MAX_TOP = {130,20};
		const SDL_Point m_SG_MAX_BOT = {1900,4120};

		// the top and bottom corner of the ship editor
		SDL_Point m_sg_top_corner = {250,100};
		SDL_Point m_sg_bot_corner;

		// top and bottom corner of menu
		const SDL_Point m_mg_top_corner = {8,8};
		const SDL_Point m_mg_bot_corner = {117,685};

		// various varibles for blurb
		const int m_blurb_width = 25;
		const int m_blurb_height = 8;
		const SDL_Point m_mb_top_corner = {8,695};
		const SDL_Point m_mb_bot_corner = {m_mb_top_corner.x + 4 + SPRITE_SIZE * m_blurb_width,m_mb_top_corner.y + 4 + SPRITE_SIZE * m_blurb_height};
		Label* m_blurb;

		// center of mass
		SDL_Point m_cm;

		// tell if point m is in the rectangle of top corner, bottom corner
		bool overlap(SDL_Point m, SDL_Point tc, SDL_Point bc);

		// initialize pointer
		void init_pointer(std::string file);	
		// initialize palette from file
		void init_palette(std::string i_path);
		// initalize the array for our ship
		void init_array();
		// render the scroll menu
		void render_menu();
		// render a small blurb below the 
		void render_blurb();
		// render the mouse pointer
		void render_pointer();
		// render the grid where ship is
		void render_ship_grid();
		// render center of mass
		void render_cm();
		// check mouse position and keys for menu
		void mouse_menu(int x, int y, Uint32 b);
		// check mouse position and keys for ship editor
		void mouse_ship(int x, int y, Uint32 b);
};

#endif
