ARGS = 
FLAGS = -lglut -lGLU -lGL -Wall -Wextra -fopenmp
HEADERS = $(wildcard libs/*.cpp)

all:
	g++ graphic.cpp -o app $(FLAGS) $(HEADERS)

run:
	./app $(ARGS)

evolve:
	g++ Evolution.cpp -o app -Wall -Wextra -fopenmp libs/ants.cpp
	./app

valgrind:
	valgrind --leak-check=yes ./app $(ARGS) 

.PHONY: clean
clean:
	rm -f app
