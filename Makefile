# the sdl libs
SDL = -lSDL2 -lSDL2_image

# compiler
CXX = g++

# debugging flags (comment if not needed)
DB_FLAGS = -Wall -g

# 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

l: main.cpp asteroid.o sprite.o entity.h frame_counter.o particle.o fighter.o editor.o
	$(CXX) $(DB_FLAGS) sprite.o particle.o label.o frame_counter.o asteroid.o editor.o fighter.o main.cpp $(SDL) -o l

editor.o: editor.h editor.cpp label.o sprite.o entity.h
	$(CXX) $(DB_FLAGS) -c editor.cpp

label.o: label.h label.cpp sprite.o entity.h
	$(CXX) $(DB_FLAGS) -c label.cpp

asteroid.o: asteroid.cpp asteroid.h particle.o sprite.o entity.h
	$(CXX) $(DB_FLAGS) -c asteroid.cpp

fighter.o: fighter.h fighter.cpp particle.o sprite.o entity.h
	$(CXX) $(DB_FLAGS) -c fighter.cpp

particle.o: particle.h particle.cpp sprite.o
	$(CXX) $(DB_FLAGS) -c particle.cpp

frame_counter.o: frame_counter.h frame_counter.cpp
	$(CXX) $(DB_FLAGS) -c frame_counter.cpp

sprite.o: sprite.cpp sprite.h
	$(CXX) $(DB_FLAGS) -c sprite.cpp

run: l
	./l

clean:
	rm *.o
	rm l
