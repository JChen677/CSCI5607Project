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

PlayerInfo gPlayer = {0, 4, 2};
PlayerInfo rPlayer = {1, 19, 17};
PlayerInfo bPlayer = {2, 34, 32};
PlayerInfo yPlayer = {3, 49, 47};

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
    piece->pos = (piece->player.startPos + spaces - 1) % 60;
  } else if (piece->place == board) {
    piece->pos = initPos + spaces;
    // If started behind and ended ahead of safety, move into safety zone
    // If started at end of board and ended ahead of safety (only happens for Green), move into safety zone
    if ((initPos <= piece->player.safetyPos) && (piece->pos > piece->player.safetyPos) ||
        (piece->pos >= 60) && (piece->pos - 60 > piece->player.safetyPos)) {
      spaces = piece->pos % 60 - piece->player.safetyPos - 1;
      if (spaces > 5) {  // Overshot, return to initial position
        piece->pos = initPos;
      } else if (spaces == 5) {  // Landed in home
        piece->place = home;
        piece->pos = -1;
      } else {  // Landed in safety
        piece->place = safety;
        piece->pos = spaces;
      }
    } else {
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

  //printf("%d\n", piece->pos);
  return piece->place;
}

// Converts position of piece into x and y coords
void getXY(Piece curr, float* x, float* y) {
  *x = -10;
  *y = -10;
  if (curr.place == start) {
    if (curr.player.num == 0) {  // Green
      if (curr.num == 1) {
        *x = -1.71;
        *y = 3.75;
      } else if (curr.num == 2) {
        *x = -2.39;
        *y = 3.75;
      } else {  // 3
        *x = -3.07;
        *y = 3.75;
      }
    } else if (curr.player.num == 1) {  // Red
      if (curr.num == 1) {
        *x = 3.75;
        *y = 1.71;
      } else if (curr.num == 2) {
        *x = 3.75;
        *y = 2.39;
      } else {  // 3
        *x = 3.75;
        *y = 3.07;
      }
    } else if (curr.player.num == 2) {  // Blue
      if (curr.num == 1) {
        *x = 1.71;
        *y = -3.75;
      } else if (curr.num == 2) {
        *x = 2.39;
        *y = -3.75;
      } else {  // 3
        *x = 3.07;
        *y = -3.75;
      }
    } else {  // 3, Yellow
      if (curr.num == 1) {
        *x = -3.75;
        *y = -1.71;
      } else if (curr.num == 2) {
        *x = -3.75;
        *y = -2.39;
      } else {  // 3
        *x = -3.75;
        *y = -3.07;
      }
    }
  } else if (curr.place == board) {
    if (curr.pos < 15) {
      *x = -5.12 + 0.68 * curr.pos;
      *y = 5.12;
    } else if (curr.pos < 30) {
      *x = 5.12;
      *y = 5.12 - 0.68 * (curr.pos - 15);
    } else if (curr.pos < 45) {
      *x = 5.12 - 0.68 * (curr.pos - 30);
      *y = -5.12;
    } else {
      *x = -5.12;
      *y = -5.12 + 0.68 * (curr.pos - 45);
    }
  } else if (curr.place == safety) {
    if (curr.player.num == 0) {  // Green
      *x = -3.75;
      *y = 4.44 - 0.68 * curr.pos;
    } else if (curr.player.num == 1) {  // Red
      *x = 4.44 - 0.68 * curr.pos;
      *y = 3.75;
    } else if (curr.player.num == 2) {  // Blue
      *x = 3.75;
      *y = -4.44 + 0.68 * curr.pos;
    } else {  // 3, Yellow
      *x = -4.44 + 0.68 * curr.pos;
      *y = -3.75;
    }
  } else {  // home
    if (curr.player.num == 0) {  // Green
      if (curr.num == 1) {
        *x = -3.41;
        *y = 0.34;
      } else if (curr.num == 2) {
        *x = -3.75;
        *y = 1.02;
      } else {  // 3
        *x = -4.09;
        *y = 0.34;
      }
    } else if (curr.player.num == 1) {  // Red
      if (curr.num == 1) {
        *x = 0.34;
        *y = 3.41;
      } else if (curr.num == 2) {
        *x = 1.02;
        *y = 3.75;
      } else {  // 3
        *x = 0.34;
        *y = 4.09;
      }
    } else if (curr.player.num == 2) {  // Blue
      if (curr.num == 1) {
        *x = 3.41;
        *y = -0.34;
      } else if (curr.num == 2) {
        *x = 3.75;
        *y = -1.02;
      } else {  // 3
        *x = 4.09;
        *y = -0.34;
      }
    } else {  // 3, Yellow
      if (curr.num == 1) {
        *x = -0.34;
        *y = -3.41;
      } else if (curr.num == 2) {
        *x = -1.02;
        *y = -3.75;
      } else {  // 3
        *x = -0.34;
        *y = -4.09;
      }
    }
  }
}

#endif  // BOARD_H_
