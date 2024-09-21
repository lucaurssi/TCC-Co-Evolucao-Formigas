FILE = main
ARGS = 
FLAGS = -lglut -lGLU -lGL -Wall -Wextra
HEADERS = $(wildcard libs/*.cpp)

all:
	g++ $(FILE).cpp -o app $(FLAGS) $(HEADERS)

run:
	./app $(ARGS)

.PHONY: clean
clean:
	rm -f app
