.PHONY: all clean
all: lander_game

lander_game:
	g++ lander.cpp -lncursesw -std=c++11 -pthread -o lander_game

clean:
	rm -f lander_game
