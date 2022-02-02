#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifndef FRAME_COUNTER_H
#define FRAME_COUNTER_H

class Frame_Counter {

	public:
		// init the counter with frame rate
		Frame_Counter(int fps);

		// will return when the next frame starts
		void wait_for_next_frame();

		// start the timer at start of game
		void start_counter();

		// get the current frame number, to check if 
		int get_frame_num();

		// returns the time since the start
		Uint32 get_time_since_start();

		// get the current time since start
		Uint32 start_timer();

		// gets the time since the given time
		Uint32 check_timer(Uint32 start);

		// tells if the time has passes
		bool check_time_up(Uint32 start);

	private:

		int m_num_frames;
		Uint32 m_start;
		Uint32 m_last_frame;
		Uint32 m_per_frame;

};

#endif
