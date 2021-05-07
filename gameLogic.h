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
  chooseMove,
  movingPiece,
  turnEnd,
  movingCamera
};

int currTurn = 0;
int displayCard = 1;
float cardposition = 0.0;
float cameraposition = 1.0;
int playerInput = -1;
int chosenPiece = -1;
TurnState state = turnBegin;
bool waiting = true;
float pieceposition = 0.0;
int movementindex = 1;


int processMovement(int pieceInd) {
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
      if (state == choosePiece) {
        printf("Choose +1, +2, +3, +4, +5, +6, or +7 movement:\n");
        printf("(1) Forward 1\n");
        printf("(2) Forward 2\n");
        printf("(3) Forward 3\n");
        printf("(4) Forward 4\n");
        printf("(5) Forward 5\n");
        printf("(6) Forward 6\n");
        printf("(7) Forward 7\n");
        state = chooseMove;
      } else {
        if (playerInput == 1) {
          movement = 1;
        } else if (playerInput == 2) {
          movement = 2;
        } else if (playerInput == 3) {
          movement = 3;
        } else if (playerInput == 4) {
          movement = 4;
        } else if (playerInput == 5) {
          movement = 5;
        } else if (playerInput == 6) {
          movement = 6;
        } else if (playerInput == 7) {
          movement = 7;
        }
      }
      break;
    case card8:
      movement = 8;
      break;
    case card10:
      if (state == choosePiece) {
        printf("Choose either +10 or -1 movement:\n");
        printf("(1) Forward 10\n");
        printf("(2) Backwards 1\n");
        state = chooseMove;
      } else {
        if (playerInput == 1) {
          movement = 10;
        } else if (playerInput == 2) {
          movement = -1;
        }
        // else input is invalid, do nothing
      }
      break;
    case card11:
      movement = 11;
      break;
    case card12:
      movement = 12;
      break;
    case cardOops:
      movement = rand() % 13 + 1;
      break;
  }

  if (movement == 0) {
    return -1;
  } else {
    movePiece(&pieces.at(pieceInd), movement);
    return 0;
  }
}

void takeTurn() {
  if (waiting) { return; }

  if (state == turnBegin) {
    displayCard = (Cards) (rand() % 11 + 2);
    state = drawingCard;
    printf("Drawing card\n");
  } else if (state == drawingCard) {
    playerInput = -1;
    // Wait for card to finish being drawn in drawGeometry()
  } else if (state == choosePiece) {
    if ((playerInput < 1) || (playerInput > 3)) { return; }
    chosenPiece = playerInput;
    playerInput = -1;

    if (processMovement(currTurn * 3 + chosenPiece - 1) == -1) { return; }
    state = movingPiece;
    printf("Moving Piece\n");
  } else if (state == chooseMove) {  // Only happens for cards 7 and 10
    if (processMovement(currTurn * 3 + chosenPiece - 1) == -1) { return; }
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
    printf("\nNEW TURN\n\n");
  } else if (state == movingCamera) {
    waiting = true;
    //cameraposition = 0.0;
  }
}

#endif  // GAME_LOGIC_H_
