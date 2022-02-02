#include "editor.h"

// default constructor
Editor::Editor(std::string file, SDL_Renderer* gRenderer, Frame_Counter* timer, int height, int width) 
	: Entity {"Tiles/" + file + "_tiles.png","", gRenderer,nullptr,nullptr,height,width} {

	// initialize our ship
	m_vis.set_center(SDL_Point {0,0});
	m_sg_bot_corner = {m_sg_top_corner.x + m_width * SPRITE_SIZE - 1,m_sg_top_corner.y + m_height * SPRITE_SIZE - 1};
	// initialize our blurb
	m_blurb = new Label(gRenderer,m_blurb_height,m_blurb_width);
	m_blurb->set_position(m_mb_top_corner.x + 2,m_mb_top_corner.y + 2);

	// iniitalize our palette
	m_palette = {};
	init_palette("Info/" + file + "_info.txt");

	// initialize some other variables
	m_type = EDITOR;
	m_renderer = gRenderer;
	m_r_pressed = false;

	// initialize center of mass
	m_cm = center_of_mass();

	// initialize the pointer
	init_pointer(file);

	// initialize timer
	m_timer = timer;

}

Editor::~Editor(){

	delete m_blurb;

}

void Editor::file_import(){

	std::ifstream fh;
	fh.open("test.shp");
	if (fh.is_open()){
		int size = sizeof(int);
		char* temp = (char*) calloc(4,sizeof(char));
		Tile temp_t;
		fh.read(temp,size);
		m_width = *reinterpret_cast<int*>(temp);
		fh.read(temp,size);
                m_height = *reinterpret_cast<int*>(temp);
              	for (int i = 0; i < m_width; i++){
                       	for (int j = 0; j < m_height; j++){
                                fh.read(temp,sizeof(Tile));
		                temp_t = *reinterpret_cast<Tile*>(temp);
				m_array[i][j] = temp_t;
                        }
                }


		delete temp;	

	} else {
		printf("Could not open file to import!\n");
	}
}

// export this ship to a file
void Editor::file_export(){

	std::ofstream fh;
	fh.open("test.shp");
	if (fh.is_open()){
		
		fh.write(reinterpret_cast<char*>(&m_width), sizeof(int));
		fh.write(reinterpret_cast<char*>(&m_height), sizeof(int));

		for (int i = 0; i < m_width; i++){
			for (int j = 0; j < m_height; j++){
				fh.write(reinterpret_cast<char*>(&(m_array[i][j])),sizeof(Tile));
			}
		}

		fh.close();
		printf("File done!\n");
	} else {
		printf("Could not open file to export!\n");
	}

}

void Editor::init_pointer(std::string file){
	// initialize our mouse pointer
	m_pointer = Sprite();
	m_pointer.set_sprite_sheet("Tiles/" + file + "_tiles.png","", m_renderer);
	m_p_array = (Tile**) calloc(1,sizeof(Tile*));
	m_p_array[0] = (Tile*) calloc(1,sizeof(Tile));
	m_p_array[0][0] = {0,0,0,0,-1,0};
	m_pointer.set_array(m_p_array,1,1);
	m_pointer.set_center(SDL_Point {12,12});
}

// get the array for the block palette
void Editor::init_palette(std::string i_path){
	std::ifstream fh;
	fh.open(i_path);
	std::string temp;
	std::string name;
	int index;
	bool can_rotate;
	int health;
	int weight;
	if (fh.is_open()){
		// how big is our palette
		getline(fh,temp);
		m_palette_size = stoi(temp);

		// load in the pallete
		for (int i = 0; i < m_palette_size; i++){
			// get the name
			getline(fh,name);

			// get the index
			getline(fh,temp);
			index = stoi(temp);

			// get whether can rotate
			getline(fh,temp);
			can_rotate = (temp == "1");

			// get health
			getline(fh,temp);
			health = stoi(temp);
			
			// get wieght
			getline(fh,temp);
			weight = stoi(temp);

			m_palette.push_back(Tile_Desc {name,"",index,can_rotate,health,weight});
			
		}
		fh.close();

	} else {
		printf("Error opening file %s.",i_path.c_str());
	}
}

// if mouse is over menu, change pointer
void Editor::mouse_menu(int x, int y, Uint32 b){

	int temp;	
	if (b & SDL_BUTTON_LMASK){
		x -= 10;
		y -= 10;
		x /= 27;
		y /= 27;
		if(x >= 0 && x < 4 && y >= 0 && y < 25){
			temp = x + y * 4;
			if (static_cast<long unsigned int>(temp) < m_palette.size()){
				m_p_array[0][0].indx = m_palette[temp].index;
				m_p_array[0][0].rotation = 0;
				m_p_array[0][0].health = m_palette[temp].max_health;
				m_p_array[0][0].weight = m_palette[temp].weight;
				m_p_array[0][0].ref_indx = temp;
				m_p_array[0][0].damage_state = 0;
				m_blurb->set_string(m_palette[temp].name);
			}
		}
	}
	

	else if (b & SDL_BUTTON_RMASK) {
		m_p_array[0][0] = {0,0,0,0,-1,0};
	}
	
}

// if mouse is over menu, change pointer
void Editor::mouse_ship(int x,int y,Uint32 b){
	
	// place block
	if (b & SDL_BUTTON_LMASK) {
		x -= m_sg_top_corner.x;
		y -= m_sg_top_corner.y;
		x /= 24;
		y /= 24;
		if (!m_array[y][x].indx){
			m_array[y][x] = m_p_array[0][0];
		}
	}

	// pick block up
	else if (b & SDL_BUTTON_RMASK) {
		x -= m_sg_top_corner.x;
		y -= m_sg_top_corner.y;
		x /= 24;
		y /= 24;
		m_array[y][x] = {0,0,0,0,-1,0};
	}
}

bool Editor::overlap(SDL_Point m, SDL_Point tc, SDL_Point bc){
	return (m.x >= tc.x && m.x <= bc.x && m.y >= tc.y && m.y <= bc.y);
}

// check keys and also events
void Editor::check_keys(const Uint8* key_states){
	int x, y;
	Uint32 mouse_state = SDL_GetMouseState(&x,&y);

	SDL_Point mp {x,y};

	// check for mouse over menu	
	if (overlap(mp, m_mg_top_corner, m_mg_bot_corner)){
		mouse_menu(x,y,mouse_state);
	}

	// check for mouse over ship editor
	if (overlap(mp,m_sg_top_corner,m_sg_bot_corner) && !overlap(mp,m_mb_top_corner,m_mb_bot_corner)){
		mouse_ship(x,y,mouse_state);
	}

	// check for keys to move ship grid
	if (key_states[SDL_SCANCODE_DOWN] && m_sg_bot_corner.y < m_SG_MAX_BOT.y){
		m_sg_top_corner.y += 3;
		m_sg_bot_corner.y += 3;	
	}
	if (key_states[SDL_SCANCODE_UP]  && m_sg_top_corner.y > m_SG_MAX_TOP.y){
		m_sg_top_corner.y -= 3;
		m_sg_bot_corner.y -= 3;	
	}
	if (key_states[SDL_SCANCODE_LEFT] && m_sg_top_corner.x > m_SG_MAX_TOP.x){
		m_sg_top_corner.x -= 3;
		m_sg_bot_corner.x -= 3;	
	}
	if (key_states[SDL_SCANCODE_RIGHT] && m_sg_bot_corner.x < m_SG_MAX_BOT.x){
		m_sg_top_corner.x += 3;
		m_sg_bot_corner.x += 3;	
	}

	// check for r to rotate
	if (key_states[SDL_SCANCODE_R]){
		if (!m_r_pressed && m_palette[m_p_array[0][0].ref_indx].can_rotate){
			m_p_array[0][0].rotation += 1;
			m_p_array[0][0].rotation %= 4;
			m_r_pressed = true;
		}
	} else {
		m_r_pressed = false;
	}
}

void Editor::render_pointer(){
	int x,y;
	SDL_GetMouseState(&x,&y);
	m_pointer.render(x,y,0);
}

void Editor::render_menu(){

	SDL_Rect grd;

	SDL_SetRenderDrawColor(m_renderer,0x18,0x19,0x6C,0xFF);
	
	grd.x = m_mg_top_corner.x;
	grd.y = m_mg_top_corner.y;
	grd.w = m_mg_bot_corner.x - m_mg_top_corner.x;
	grd.h = m_mg_bot_corner.y - m_mg_top_corner.y;

	SDL_RenderFillRect(m_renderer,&grd);

	grd.h = 28;
	grd.w = 109;
	grd.x = m_mg_top_corner.x;
	SDL_SetRenderDrawColor(m_renderer,0x95,0x95,0x95,0xff);

	// draw the grid for blocks
	for (int i = 0; i < 25; i ++){
		grd.y = m_mg_top_corner.y + 27 * i;
		SDL_RenderDrawRect(m_renderer,&grd);
	}

	// draw the verticle lines
	SDL_Point pts[2] = {{35,8},{35,683}};
	SDL_RenderDrawLines(m_renderer,pts,2);
	pts[0].x +=27;
	pts[1].x +=27;
	SDL_RenderDrawLines(m_renderer,pts,2);
	pts[0].x +=27;
	pts[1].x +=27;
	SDL_RenderDrawLines(m_renderer,pts,2);

	SDL_SetRenderDrawColor(m_renderer,0x00,0x00,0x00,0xff);


        int num;
	Tile temp = {0,0,0,0,0,0};

        // rectangle where we are getting tile
        SDL_Rect src;
        src.h = SPRITE_SIZE;
        src.w = SPRITE_SIZE;

        // rectangle where we are putting tile
        SDL_Rect dst;
        dst.h = SPRITE_SIZE;
        dst.w = SPRITE_SIZE;

	// draw the actual blocks on the grid
	for (int i = 0; i < m_palette_size; i++){
		temp.indx = m_palette[i].index;
	
		num = temp.indx;

		// get our image from sprite sheet
		src.x = (num % SPRITE_SHEET_WIDTH) * SPRITE_SIZE;
		src.y = (num / SPRITE_SHEET_WIDTH) * SPRITE_SIZE;
	
		dst.x = 10 + (SPRITE_SIZE + 3) * (i % 4);
		dst.y = 10 + (SPRITE_SIZE + 3) * (i / 4);

		SDL_RenderCopy(m_renderer,m_vis.get_texture(),&src,&dst);
	}
}

void Editor::render_blurb(){

	SDL_Rect grd;
	grd.x = m_mb_top_corner.x;
	grd.y = m_mb_top_corner.y;
	grd.h = m_mb_bot_corner.y - m_mb_top_corner.y;
	grd.w = m_mb_bot_corner.x - m_mb_top_corner.x;
	SDL_SetRenderDrawColor(m_renderer,0x95,0x95,0x95,0xFF);
	SDL_RenderDrawRect(m_renderer,&grd);
	SDL_SetRenderDrawColor(m_renderer,0x18,0x19,0x6C,0xFF);
	grd.x++;
	grd.y++;
	grd.h -= 2;
	grd.w -= 2;
	SDL_RenderFillRect(m_renderer,&grd);

	SDL_SetRenderDrawColor(m_renderer,0x00,0x00,0x00,0xFF);

	m_blurb->draw(0,0);
}

// render the grid for where the ship is placed
void Editor::render_ship_grid(){

	SDL_Rect grd;
	grd.h = 24;
	grd.w = 24;
	SDL_SetRenderDrawColor(m_renderer,0x50,0x50,0x50,0xff);
	for (int i = 0; i < m_width; i++){
		for (int j = 0; j < m_height; j++){
			grd.x = m_sg_top_corner.x + i * SPRITE_SIZE;
			grd.y = m_sg_top_corner.y + j * SPRITE_SIZE;
			SDL_RenderDrawRect(m_renderer,&grd);
		}
	}
	SDL_SetRenderDrawColor(m_renderer,0x00,0x00,0x00,0xFF);

}

// draw center of mass to grid
void Editor::render_cm(){

	SDL_SetRenderDrawColor(m_renderer,0xD0,0xB8,0x00,0xFF);

	SDL_Rect grd;
	grd.h = 4;
	grd.w = 4;
	grd.x = m_cm.y - 2 + m_sg_top_corner.x;
	grd.y = m_cm.x - 2 + m_sg_top_corner.y;
	SDL_RenderDrawRect(m_renderer,&grd);

	SDL_SetRenderDrawColor(m_renderer,0x00,0x00,0x00,0xff);

}

// draw ship and editor
void Editor::draw(double x, double y){
	render_ship_grid();
	m_vis.render(m_sg_top_corner.x,m_sg_top_corner.y,0);
	render_menu();
	render_blurb();
	render_cm();
	render_pointer();
}

void Editor::update(){

	if (m_timer->get_frame_num() % 15 == 0) {
		m_cm = center_of_mass();
	}

}
