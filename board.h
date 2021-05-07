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

glm::vec3 pawnColors[] = {
  glm::vec3(0, 0.375, 0), glm::vec3(0, 1, 0), glm::vec3(0.625, 1, 0.625),
  glm::vec3(0.375, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0.625, 0.625),
  glm::vec3(0, 0, 0.375), glm::vec3(0, 0, 1), glm::vec3(0.625, 0.625, 1),
  glm::vec3(0.375, 0.375, 0), glm::vec3(1, 1, 0), glm::vec3(1, 1, 0.625)
};

glm::vec3 camPos[] = {
  glm::vec3(0.f, 15, 9),
  glm::vec3(15, 0.f, 9),
  glm::vec3(0.f, -15, 9),
  glm::vec3(-15, 0.f, 9)
};

struct Slide {  // Represent slides
  int start;  // Beginning of slide, using same positioning as Piece
  int end;    // End of slide
};

// Define global variables
std::vector<Slide> slides;
std::vector<Piece> pieces;
std::vector<Light> lights;
std::vector<glm::vec3> movepath;
glm::vec3 slidStart;
glm::vec3 slidPos;
bool slidCheck = false;
glm::vec3 oopsPos;
bool oopsCheck = false;
int movedpiece = -1;
int movedplayer = -1;
int oopspiece = -1;
int oopsplayer = -1;

void altgetXY(Placement place, int pos, int num, int player,  float* x, float* y) {
  *x = -10;
  *y = -10;
  if (place == start) {
    if (player == 0) {  // Green
      if (num == 1) {
        *x = -1.71;
        *y = 3.75;
      } else if (num == 2) {
        *x = -2.39;
        *y = 3.75;
      } else {  // 3
        *x = -3.07;
        *y = 3.75;
      }
    } else if (player == 1) {  // Red
      if (num == 1) {
        *x = 3.75;
        *y = 1.71;
      } else if (num == 2) {
        *x = 3.75;
        *y = 2.39;
      } else {  // 3
        *x = 3.75;
        *y = 3.07;
      }
    } else if (player == 2) {  // Blue
      if (num == 1) {
        *x = 1.71;
        *y = -3.75;
      } else if (num == 2) {
        *x = 2.39;
        *y = -3.75;
      } else {  // 3
        *x = 3.07;
        *y = -3.75;
      }
    } else {  // 3, Yellow
      if (num == 1) {
        *x = -3.75;
        *y = -1.71;
      } else if (num == 2) {
        *x = -3.75;
        *y = -2.39;
      } else {  // 3
        *x = -3.75;
        *y = -3.07;
      }
    }
  } else if (place == board) {
    if (pos < 15) {
      *x = -5.12 + 0.68 * pos;
      *y = 5.12;
    } else if (pos < 30) {
      *x = 5.12;
      *y = 5.12 - 0.68 * (pos - 15);
    } else if (pos < 45) {
      *x = 5.12 - 0.68 * (pos - 30);
      *y = -5.12;
    } else {
      *x = -5.12;
      *y = -5.12 + 0.68 * (pos - 45);
    }
  } else if (place == safety) {
    if (player == 0) {  // Green
      *x = -3.75;
      *y = 4.44 - 0.68 * pos;
    } else if (player == 1) {  // Red
      *x = 4.44 - 0.68 * pos;
      *y = 3.75;
    } else if (player == 2) {  // Blue
      *x = 3.75;
      *y = -4.44 + 0.68 * pos;
    } else {  // 3, Yellow
      *x = -4.44 + 0.68 * pos;
      *y = -3.75;
    }
  } else {  // home
    if (player == 0) {  // Green
      if (num == 1) {
        *x = -3.41;
        *y = 0.34;
      } else if (num == 2) {
        *x = -3.75;
        *y = 1.02;
      } else {  // 3
        *x = -4.09;
        *y = 0.34;
      }
    } else if (player == 1) {  // Red
      if (num == 1) {
        *x = 0.34;
        *y = 3.41;
      } else if (num == 2) {
        *x = 1.02;
        *y = 3.75;
      } else {  // 3
        *x = 0.34;
        *y = 4.09;
      }
    } else if (player == 2) {  // Blue
      if (num == 1) {
        *x = 3.41;
        *y = -0.34;
      } else if (num == 2) {
        *x = 3.75;
        *y = -1.02;
      } else {  // 3
        *x = 4.09;
        *y = -0.34;
      }
    } else {  // 3, Yellow
      if (num == 1) {
        *x = -0.34;
        *y = -3.41;
      } else if (num == 2) {
        *x = -1.02;
        *y = -3.75;
      } else {  // 3
        *x = -0.34;
        *y = -4.09;
      }
    }
  }
}

// Moves a piece
// Takes pointer to moving piece and number of space to move
// Returns which game space the piece ended on
Placement movePiece(Piece* piece, int spaces) {
  int initPos = piece->pos;  // In case we need to reset position
  float x, y;
  movedpiece = piece->num;
  movedplayer = piece->player.num;
  altgetXY(piece->place,initPos,piece->num,piece->player.num,&x,&y);
  movepath.push_back(glm::vec3(x,y,0));
  // Initial movement
  if ((piece->place == start) && (spaces > 0)) {  // Not allowed to move backwards out of start
    piece->place = board;
    piece->pos = (piece->player.startPos + spaces - 1) % 60;
    for (int i = piece->player.startPos; i != piece->pos; i = (i + 1) % 60) {
      altgetXY(piece->place,i,piece->num,piece->player.num,&x,&y);
      movepath.push_back(glm::vec3(x,y,0));
    }
    altgetXY(piece->place,piece->pos,piece->num,piece->player.num,&x,&y);
    movepath.push_back(glm::vec3(x,y,0));
    // Check for sliding
    // Probably only necessary bc of oops = 13, but stay here just in case
    for (int i = 0; i < slides.size(); i++) {
      if (piece->pos == slides.at(i).start) {
        piece->pos = slides.at(i).end;
        altgetXY(piece->place,piece->pos,piece->num,piece->player.num,&x,&y);
        slidCheck = true;
        slidPos = glm::vec3(x,y,0);
      }
    }
  } else if (piece->place == board) {
    piece->pos = initPos + spaces;
    // If started behind and ended ahead of safety, move into safety zone
    // If started at end of board and ended ahead of safety (only happens for Green), move into safety zone
    if ((initPos <= piece->player.safetyPos) && (piece->pos > piece->player.safetyPos) ||
        (piece->pos >= 60) && (piece->pos - 60 > piece->player.safetyPos)) {
      int oldspaces = spaces;
      spaces = piece->pos % 60 - piece->player.safetyPos - 1;
      if (spaces > 5) {  // Overshot, return to initial position
        piece->pos = initPos;
        altgetXY(piece->place,piece->pos,piece->num,piece->player.num,&x,&y);
        movepath.push_back(glm::vec3(x,y,0));
      } 
      else {
        if (oldspaces > 0) {
          for (int i = (initPos + 1) % 60; i != piece->player.safetyPos; i = (i + 1) % 60) {
            altgetXY(board,i,piece->num,piece->player.num,&x,&y);
            movepath.push_back(glm::vec3(x,y,0));
          }
        }
        else {
          for (int i = (initPos - 1); i != piece->player.safetyPos; i--) {
              if (i < 0) {
                i += 60;
              }
              altgetXY(board,i,piece->num,piece->player.num,&x,&y);
              movepath.push_back(glm::vec3(x,y,0));
            }
        }
        altgetXY(board,piece->player.safetyPos,piece->num,piece->player.num,&x,&y);
        movepath.push_back(glm::vec3(x,y,0));
        if (spaces == 5) {  // Landed in home
          piece->place = home;
          piece->pos = -1;
          for (int i = 0; i < 5; i++) {
            altgetXY(safety,i,piece->num,piece->player.num,&x,&y);
            movepath.push_back(glm::vec3(x,y,0));
          }
          altgetXY(home,-1,piece->num,piece->player.num,&x,&y);
          movepath.push_back(glm::vec3(x,y,0));
        } else {  // Landed in safety
          piece->place = safety;
          piece->pos = spaces;
          for (int i = 0; i <= piece->pos; i++) {
            altgetXY(safety,i,piece->num,piece->player.num,&x,&y);
            movepath.push_back(glm::vec3(x,y,0));
          }
        }
      }
      
    } else {
      piece->pos = (piece->pos + 60) % 60;
      if (spaces > 0) {
        for (int i = (initPos + 1) % 60; i != piece->pos; i = (i + 1) % 60) {
          altgetXY(piece->place,i,piece->num,piece->player.num,&x,&y);
          movepath.push_back(glm::vec3(x,y,0));
        }
      }
      else {
        for (int i = initPos - 1; i != piece->pos; i = (i + 59) % 60) {
          if (i == -1) { i = 59; }
          altgetXY(piece->place,i,piece->num,piece->player.num,&x,&y);
          movepath.push_back(glm::vec3(x,y,0));
          printf("(i, x, y): %d, %f, %f\n", i, x, y);
          printf("\tpos: %d\n", piece->pos);
        }
      }
      altgetXY(piece->place,piece->pos,piece->num,piece->player.num,&x,&y);
      movepath.push_back(glm::vec3(x,y,0));
      // Check for sliding
      for (int i = 0; i < slides.size(); i++) {
        if (piece->pos == slides.at(i).start) {
          piece->pos = slides.at(i).end;
          altgetXY(piece->place,piece->pos,piece->num,piece->player.num,&x,&y);
          slidCheck = true;
          slidPos = glm::vec3(x,y,0);
        }
      }
    }
  } else if (piece->place == safety) {
    piece->pos = piece->pos + spaces;
    if (piece->pos > 5 || piece->pos < 0) {  // Overshot, return to initial position
      piece->pos = initPos;
      altgetXY(piece->place,piece->pos,piece->num,piece->player.num,&x,&y);
      movepath.push_back(glm::vec3(x,y,0));
    } else if (piece->pos == 5) {  // Landed in home
      piece->place = home;
      piece->pos = -1;
      for (int i = initPos + 1; i < 5; i++) {
        altgetXY(safety,i,piece->num,piece->player.num,&x,&y);
        movepath.push_back(glm::vec3(x,y,0));
      }
      altgetXY(piece->place,piece->pos,piece->num,piece->player.num,&x,&y);
      movepath.push_back(glm::vec3(x,y,0));
    } else {
      if (piece->pos == 0) {
        for (int i = initPos - 1; i != 0; i--) {
          altgetXY(safety,i,piece->num,piece->player.num,&x,&y);
          movepath.push_back(glm::vec3(x,y,0));
        }
        altgetXY(piece->place,0,piece->num,piece->player.num,&x,&y);
        movepath.push_back(glm::vec3(x,y,0));
      }
      else {
        for (int i = initPos + 1; i != piece->pos; i++) {
          altgetXY(safety,i,piece->num,piece->player.num,&x,&y);
          movepath.push_back(glm::vec3(x,y,0));
        }
        altgetXY(piece->place,piece->pos,piece->num,piece->player.num,&x,&y);
        movepath.push_back(glm::vec3(x,y,0));
      }
    }
  } else {
    altgetXY(piece->place,piece->pos,piece->num,piece->player.num,&x,&y);
    movepath.push_back(glm::vec3(x,y,0));
  }// else piece is already in home, do nothing

  // Check for piece collision
  // TODO: Home Guard
  for (int i = 0; i < pieces.size(); i++) {
    Piece* other = &pieces.at(i);
    if ((piece->player.num == other->player.num) && (piece->num == other->num)){
      continue;
    } else if ((piece->place == other->place) && (piece->pos == other->pos) && (other->place != home) && (other->place != start) && (other->place != safety || (other->place == safety && other->player.num == piece->player.num))) {
      altgetXY(other->place,other->pos,other->num,other->player.num,&x,&y);
      oopsPos = glm::vec3(x,y,0);
      other->place = start;
      other->pos = -1;
      oopspiece = other->num;
      oopsplayer = other->player.num;
      oopsCheck = true;
    }
  }

  //printf("%d\n", piece->pos);
  for (int i = 0; i < movepath.size(); i++) {
    printf("%f x, %f y\n",movepath.at(i).x,movepath.at(i).y);
  }
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
