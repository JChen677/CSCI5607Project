#ifndef BOARD_H_
#define BOARD_H_

#include <vector>

enum Placement {
  start,
  board,
  safety,
  home
};

struct Piece {  // Represent game pieces
  int player;       // What player it belongs to
  int num;          // Which piece it is
  Placement place;  // If Piece is at start, on board, in safety zone, or home
  int pos;          // If Piece is on board, where it is (0 for first square, 59 for last square)
                    //  If Piece is in safety zone, which square it is (0 for start and 4 for end, 5 is home)
};

// IMPLEMENT LATER
struct Slide {  // Represent slides
  int start;  // Beginning of slide, using same positioning as Piece
  int end;    // End of slide
};

std::vector<Slide> slides;
slides.push_back({1, 4});
slides.push_back({9, 13});
slides.push_back({16, 19});
slides.push_back({24, 28});
slides.push_back({31, 34});
slides.push_back({39, 43});
slides.push_back({46, 49});
slides.push_back({54, 58});

#endif  // BOARD_H_
