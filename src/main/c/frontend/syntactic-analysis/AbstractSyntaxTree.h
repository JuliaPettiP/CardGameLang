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
typedef struct PlayRule PlayRule;
typedef struct PlayRuleList PlayRuleList;
typedef struct ColorList ColorList;
typedef struct CardAttribute CardAttribute;
typedef struct CardAttributeList CardAttributeList;
typedef struct ActionNameList ActionNameList;

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
    CardAttributeList * attributes;   /* may be NULL */
};

struct CardList {
    Card * card;
    struct CardList * next;
};

/* ------------------------------------------------------------------ */
/*  Card attributes (P1)                                               */
/* ------------------------------------------------------------------ */

/* Color list: used for both "color { red, blue }" and "color gold"   */
struct ColorList {
    char * color;               /* strdup'd color name                */
    struct ColorList * next;    /* NULL for last element              */
};

typedef enum {
    CARD_ATTR_COLOR,    /* color { red, blue } or color gold         */
    CARD_ATTR_VALUE,    /* value 0..9 or value N                     */
    CARD_ATTR_POINTS,   /* points 1..3 or points N                   */
    CARD_ATTR_EFFECT    /* effect skip_next_player or effect draw 2  */
} CardAttributeType;

struct CardAttribute {
    CardAttributeType type;
    /* CARD_ATTR_COLOR */
    ColorList * colors;     /* one element for single, many for list */
    /* CARD_ATTR_VALUE and CARD_ATTR_POINTS (share same fields)      */
    int rangeMin;
    int rangeMax;           /* == rangeMin for a single value        */
    /* CARD_ATTR_EFFECT */
    char * effectName;      /* strdup'd effect identifier            */
    int effectCount;        /* 0 when no integer argument            */
};

struct CardAttributeList {
    CardAttribute * attribute;
    struct CardAttributeList * next;
};

/* ------------------------------------------------------------------ */
/*  Declared actions (P2)                                              */
/* ------------------------------------------------------------------ */

struct ActionNameList {
    char * name;                    /* strdup'd action name           */
    struct ActionNameList * next;
};

/* ------------------------------------------------------------------ */
/*  Turn / Actions                                                      */
/* ------------------------------------------------------------------ */

typedef enum {
    TURN_ACTION_PLAIN,  /* plain statement inside if/else block: draw 1   */
    TURN_ACTION_MAY,    /* optional action at top level: may play 1        */
    TURN_ACTION_MUST    /* mandatory action at top level: must play 1      */
} TurnActionType;

struct TurnAction {
    TurnActionType type;
    char * name;   /* action identifier, e.g. "play", "draw"              */
    int count;     /* optional integer argument; 0 when not specified      */
};

struct TurnActionList {
    TurnAction * action;
    struct TurnActionList * next;
};

typedef enum {
    TURN_SIMPLE,       /* turn { must play 1 / may draw 1 / … }           */
    TURN_CONDITIONAL   /* turn { if cannot_play { … } else { … } }        */
} TurnType;

struct Turn {
    TurnType type;
    TurnActionList * statements;  /* TURN_SIMPLE: the statement list       */
    TurnActionList * ifBlock;     /* TURN_CONDITIONAL: if-cannot_play body */
    TurnActionList * elseBlock;   /* TURN_CONDITIONAL: else body           */
};

/* ------------------------------------------------------------------ */
/*  Play rules                                                          */
/* ------------------------------------------------------------------ */

typedef enum {
    PLAY_RULE_ALLOW,       /* allow ... */
    PLAY_RULE_CANNOT_PLAY  /* cannot_play ... */
} PlayRulePermission;

typedef enum {
    PLAY_CONDITION_SAME_COLOR,   /* allow if same_color                      */
    PLAY_CONDITION_SAME_VALUE,   /* allow if same_value                      */
    PLAY_CONDITION_WILD,         /* allow if wild                            */
    PLAY_CONDITION_ANY_CARD,     /* allow if any_card                        */
    PLAY_CONDITION_PLAYED_CARD   /* allow/cannot_play X if played Y          */
} PlayConditionType;

struct PlayRule {
    PlayRulePermission permission;  /* ALLOW or CANNOT_PLAY                  */
    char * subject;                 /* card name for PLAYED_CARD; else NULL  */
    PlayConditionType condition;
    char * conditionTarget;         /* card name after "if played"; else NULL */
};

struct PlayRuleList {
    PlayRule * rule;
    struct PlayRuleList * next;
};

/* ------------------------------------------------------------------ */
/*  Game                                                                */
/* ------------------------------------------------------------------ */

struct Game {
    char * name;
    PlayerRange * players;
    CardList * deck;
    int handSize;
    PlayRuleList * playRules;
    Turn * turn;
    ActionNameList * declaredActions;  /* may be NULL */
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
void destroyPlayRule(PlayRule * rule);
void destroyPlayRuleList(PlayRuleList * list);
void destroyColorList(ColorList * list);
void destroyCardAttribute(CardAttribute * attribute);
void destroyCardAttributeList(CardAttributeList * list);
void destroyActionNameList(ActionNameList * list);
void destroyPlayerRange(PlayerRange * range);
void destroyCard(Card * card);
void destroyCardList(CardList * list);
void destroyGame(Game * game);
void destroyProgram(Program * program);

#endif