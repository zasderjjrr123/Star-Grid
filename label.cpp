#include "label.h"

Label::Label(SDL_Renderer* gRenderer, int width, int height)
	: Entity {"Tiles/letter_tiles.png","",gRenderer,nullptr,nullptr,width,height}{

	Entity::m_center = {0,0};
	m_vis.set_center(m_center);
	Entity::m_radius = 0;

}

void Label::update(){}

void Label::draw(double x, double y){

	m_vis.render(m_p_x,m_p_y,0);
}

void Label::set_position(double p_x, double p_y){
	m_p_x = p_x;
	m_p_y = p_y;
}

int Label::next_space(std::string& str, int p){

	for (int i = p; p < static_cast<int>(str.size()); i++){
		if (str.at(i) == ' ' || str.at(i) == '-'){
			return i - p;
		}		
	}
	// if we got through with no spaces, return end of string
	return (str.size() - 1);

}

void Label::set_string(std::string str){
	
	for (int i = 0; i < m_width; i++){
		for (int j = 0; j < m_height; j++){
			m_array[i][j].indx = 0;
		}
	}	

	int chr = 0;

	for (int i = 0; i < static_cast<int>(str.size()); i++){
		
		if (i > (m_width * m_height)){
			break;
		}

		chr = static_cast<int>(str.c_str()[i]) - 32;

		m_array[i / m_height][i % m_height].indx = chr;

	}

}
