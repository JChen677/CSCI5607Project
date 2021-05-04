#ifndef GAME_LOGIC_H_
#define GAME_LOGIC_H_

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

enum TurnState {
  turnBegin,  // 0
  drawingCard,
  choosePiece,
  movingPiece,
  turnEnd
};

int currTurn = 0;
int displayCard = 1;
float cardposition = 0.0;
int chosenPiece = -1;
TurnState state = turnBegin;
bool waiting = true;


void takeTurn() {
  if (waiting) { return; }

  if (state == turnBegin) {
    displayCard = (Cards) (rand() % 11 + 2);
    state = drawingCard;
    printf("Drawing card\n");
  } else if (state == drawingCard) {
    chosenPiece = -1;
    // Wait for card to finish being drawn in drawGeometry()
  } else if (state == choosePiece) {
    if (chosenPiece == -1) { return; }

    int pieceInd = currTurn * 3 + chosenPiece - 1;
    movePiece(&pieces.at(pieceInd), displayCard);
    state = movingPiece;
    printf("Moving Piece\n");
  } else if (state == movingPiece) {
    // TODO: Wait for piece to finish moving
    // Will likely be formatted the same as drawingCard
    state = turnEnd;
    waiting = true;
  } else if (state == turnEnd) {
    // Reset variables
    cardposition = 0.0;
    state = turnBegin;
    waiting = true;

    currTurn = (currTurn + 1) % 4;
    printf("\n\nNEW TURN\n\n");
  }
}

#endif  // GAME_LOGIC_H_
