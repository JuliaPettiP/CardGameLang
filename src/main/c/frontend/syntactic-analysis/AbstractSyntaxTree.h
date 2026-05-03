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

// Estrutura para os jogadores (ex: 2 ou 2 .. 4)
struct PlayerRange {
    int min;
    int max;
};

// O nosso jogo principal
struct Game {
    char * name;
    PlayerRange * players;
    int handSize;
};

struct Program {
    Game * game;
};

void destroyPlayerRange(PlayerRange * range);
void destroyGame(Game * game);
void destroyProgram(Program * program);

#endif