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
  turnEnd,
  movingCamera
};

int currTurn = 0;
int displayCard = 1;
float cardposition = 0.0;
float cameraposition = 1.0;
float pieceposition = 0.0;
int movementindex = 1;
int chosenPiece = -1;
TurnState state = turnBegin;
bool waiting = true;


void processMovement(int pieceInd) {
  int movement = 0;
  switch (displayCard) {
    case card1:
      movement = 1;
      break;
    case card2:
      movement = 2;
      break;
    case card3:
      movement = 3;
      break;
    case card4:
      movement = -4;
      break;
    case card5:
      movement = 5;
      break;
    case card7:
      movement = 7;
      break;
    case card8:
      movement = 8;
      break;
    case card10:
      movement = 10;
      break;
    case card11:
      movement = 11;
      break;
    case card12:
      movement = 12;
      break;
    case cardOops:
      movement = 13;
      break;
  }

  movePiece(&pieces.at(pieceInd), movement);
}

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

    processMovement(currTurn * 3 + chosenPiece - 1);
    state = movingPiece;
    printf("Moving Piece\n");
  } else if (state == movingPiece) {
    // TODO: Wait for piece to finish moving
    // Will likely be formatted the same as drawingCard
    //state = turnEnd;
    //movementindex = 1;
    waiting = true;
  } else if (state == turnEnd) {
    // Reset variables
    cardposition = 0.0;
    displayCard = 1;
    state = movingCamera;
    waiting = true;

    currTurn = (currTurn + 1) % 4;
    cameraposition = 0.0;
    printf("\n\nNEW TURN\n\n");
  } else if (state == movingCamera) {
    waiting = true;
    //cameraposition = 0.0;
  }
}

#endif  // GAME_LOGIC_H_
