#ifndef GAME_LOGIC_H_
#define GAME_LOGIC_H_

#if defined(__APPLE__) || defined(__linux__)
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <stdlib.h>

enum Cards {
  card1 = 2,
  card2,
  card3,
  card4,
  card5,
  card7,
  card8,
  card10,
  card11,
  card12,
  cardOops  // 12
};

int currTurn = 0;
int chosenPiece = -1;


Cards drawCard() {
  Cards drawn = (Cards) (rand() % 11 + 2);
  return drawn;
}

void takeTurn() {
  // Reset tracker vars
  chosenPiece = -1;

  Cards drawn = drawCard();
  currTurn = (currTurn + 1) % 4;
}


#endif  // GAME_LOGIC_H_
