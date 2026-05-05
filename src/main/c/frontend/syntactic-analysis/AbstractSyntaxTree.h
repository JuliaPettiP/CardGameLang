#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

typedef struct Program Program;
typedef struct Game Game;
typedef struct PlayerRange PlayerRange;
typedef struct Card Card;
typedef struct CardList CardList;
typedef struct WinCondition WinCondition;
typedef struct TurnAction TurnAction;
typedef struct TurnActionList TurnActionList;
typedef struct Turn Turn;

/* ------------------------------------------------------------------ */
/*  Win condition                                                       */
/* ------------------------------------------------------------------ */

typedef enum {
    WIN_EMPTY_HAND,   /* win if empty_hand  */
    WIN_REACH_POINTS  /* win if reach_points <N> */
} WinConditionType;

struct WinCondition {
    WinConditionType type;
    int points;   /* only used when type == WIN_REACH_POINTS */
};

/* ------------------------------------------------------------------ */
/*  Players                                                             */
/* ------------------------------------------------------------------ */

struct PlayerRange {
    int min;
    int max;
};

/* ------------------------------------------------------------------ */
/*  Cards / Deck                                                        */
/* ------------------------------------------------------------------ */

struct Card {
    char * name;
};

struct CardList {
    Card * card;
    struct CardList * next;
};

/* ------------------------------------------------------------------ */
/*  Turn / Actions                                                      */
/* ------------------------------------------------------------------ */

typedef enum {
    ACTION_MAY,   /* may <name> — optional action */
    ACTION_MUST   /* must <name> — mandatory action */
} TurnActionType;

struct TurnAction {
    TurnActionType type;  /* MAY or MUST */
    char * name;          /* action identifier, e.g. "play", "draw" */
};

struct TurnActionList {
    TurnAction * action;
    struct TurnActionList * next;
};

struct Turn {
    TurnActionList * actions;
};

/* ------------------------------------------------------------------ */
/*  Game                                                                */
/* ------------------------------------------------------------------ */

struct Game {
    char * name;
    PlayerRange * players;
    int handSize;
    CardList * deck;
    Turn * turn;              /* may be NULL if section is absent */
    WinCondition * winCondition;
};

/* ------------------------------------------------------------------ */
/*  Program (root)                                                      */
/* ------------------------------------------------------------------ */

struct Program {
    Game * game;
};

/* ------------------------------------------------------------------ */
/*  Destructors                                                         */
/* ------------------------------------------------------------------ */

void destroyWinCondition(WinCondition * winCondition);
void destroyTurnAction(TurnAction * action);
void destroyTurnActionList(TurnActionList * list);
void destroyTurn(Turn * turn);
void destroyPlayerRange(PlayerRange * range);
void destroyCard(Card * card);
void destroyCardList(CardList * list);
void destroyGame(Game * game);
void destroyProgram(Program * program);

#endif
