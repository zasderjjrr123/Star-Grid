#include "frame_counter.h"

// initialize with a framerate
Frame_Counter::Frame_Counter(int fps){
	
	m_per_frame = static_cast<Uint32>((1000.0 / static_cast<float>(fps)) + 0.5f);
	m_num_frames = 0;

}

// returns when next frame begins
void Frame_Counter::wait_for_next_frame(){

	// get the time that we need to end
	Uint32 time_to_wait = (m_last_frame + m_per_frame);

	// wait until
	while (time_to_wait >= SDL_GetTicks()){
		SDL_Delay(1);
	}

	m_last_frame = SDL_GetTicks();
	m_num_frames++;

}

// start the timer
void Frame_Counter::start_counter(){
	m_start = SDL_GetTicks();
	m_last_frame = m_start;
}

// get number of frames since start
int Frame_Counter::get_frame_num(){
	return m_num_frames;
}

// gets the time since start
Uint32 Frame_Counter::get_time_since_start(){
	return SDL_GetTicks() - m_start;
}

// get the current time since start
Uint32 Frame_Counter::start_timer(){
	return SDL_GetTicks();
}

// gets the time since the given time
Uint32 Frame_Counter::check_timer(Uint32 start){
	return SDL_GetTicks() - start;
}

// check if time is up
bool Frame_Counter::check_time_up(Uint32 start){
	return SDL_GetTicks() > start;
}
