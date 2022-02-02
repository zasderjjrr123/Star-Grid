#include "entity.h"

#ifndef LABEL_H
#define LABEL_H

class Label : public Entity{

	public:
		Label(SDL_Renderer* gRenderer, int width, int height);
		void update();
		void draw(double x, double y);
		void set_string(std::string str);
		void set_position(double p_x, double p_y);

	private:
	
		// returns the index of next space from position p
		int next_space(std::string& str, int p);

};

#endif
