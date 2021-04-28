#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include "light.cpp"

enum Placement {
  start,
  board,
  safety,
  home
};

struct PlayerInfo {  // Represent player color, as well as start/end positions
  int num;  // G=0, R=1, B=2, Y=3
  int r;
  int g;
  int b;
  int startPos;
  int safetyPos;
};

struct Piece {  // Represent game pieces
  PlayerInfo player;  // Info of owning player
  int num;            // Which piece it is (1-3)
  Placement place;    // If Piece is at start, on board, in safety zone, or home
  int pos;            // If Piece is on board, where it is (0 for first square, 59 for last square)
                      //  If Piece is in safety zone, which square it is (0 for start and 4 for end, 5 is home)
};

PlayerInfo gPlayer = {0, 0, 1, 0, 4, 2};
PlayerInfo rPlayer = {1, 1, 0, 0, 19, 17};
PlayerInfo bPlayer = {2, 0, 0, 1, 34, 32};
PlayerInfo yPlayer = {3, 1, 1, 0, 49, 47};

// IMPLEMENT LATER
struct Slide {  // Represent slides
  int start;  // Beginning of slide, using same positioning as Piece
  int end;    // End of slide
};

// Define global variables
std::vector<Slide> slides;
std::vector<Piece> pieces;
std::vector<Light> lights;


// Moves a piece
// Takes pointer to moving piece and number of space to move
// Returns which game space the piece ended on
Placement movePiece(Piece* piece, int spaces) {
  int initPos = piece->pos;  // In case we need to reset position

  // Initial movement
  if (piece->place == start) {
    piece->place = board;
    piece->pos = (piece->player.startPos + spaces) % 60;
  } else if (piece->place == board) {
    piece->pos = initPos + spaces;
    if (piece->pos > piece->player.safetyPos) {  // Move into safety zone
      spaces = piece->player.safetyPos - piece->pos;
      if (spaces > 5) {  // Overshot, return to initial position
        piece->pos = initPos;
      } else if (spaces == 5) {  // Landed in home
        piece->place = home;
        piece->pos = -1;
      } else {  // Landed in safety
        piece->place = safety;
        piece->pos = spaces;
      }
    }
    piece->pos = piece->pos % 60;
    if (piece->pos < 0) {
      piece->pos += 60;
    }
    // Check for sliding
    for (int i = 0; i < slides.size(); i++) {
      if (piece->pos == slides.at(i).start) {
        piece->pos = slides.at(i).end;
      }
    }
  } else if (piece->place == safety) {
    piece->pos = piece->pos + spaces;
    if (piece->pos > 5) {  // Overshot, return to initial position
      piece->pos = initPos;
    } else if (piece->pos == 5) {  // Landed in home
      piece->place = home;
      piece->pos = -1;
    }
  } // else piece is already in home, do nothing

  return piece->place;
}

#endif  // BOARD_H_
