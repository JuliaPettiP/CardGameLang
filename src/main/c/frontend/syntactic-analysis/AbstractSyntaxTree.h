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

// Estrutura para os jogadores
struct PlayerRange {
    int min;
    int max;
};

// Estrutura para uma Carta
struct Card {
    char * name;
};

// Lista ligada de Cartas (o Baralho)
struct CardList {
    Card * card;
    struct CardList * next;
};

// O Jogo Principal
struct Game {
    char * name;
    PlayerRange * players;
    int handSize;
    CardList * deck;   // O Baralho!
    int winCondition;  // Condição de vitória (1 ou 0)
};

struct Program {
    Game * game;
};

// Declaração dos destrutores (crítico para o AddressSanitizer)
void destroyPlayerRange(PlayerRange * range);
void destroyCard(Card * card);
void destroyCardList(CardList * list);
void destroyGame(Game * game);
void destroyProgram(Program * program);

#endif