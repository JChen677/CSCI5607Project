all: multiObjectTexture.cpp
	g++ multiObjectTexture.cpp -x c glad/glad.c -g -lSDL2 -lSDL2main -lGL -ldl -I/usr/include/SDL2/ -o sorry
