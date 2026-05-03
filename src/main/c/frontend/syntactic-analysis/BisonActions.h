#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);

Program * GameProgramSemanticAction(Game * game);
PlayerRange * PlayerRangeSemanticAction(const int min, const int max);
Card * CardSemanticAction(char * name);
CardList * CardListSemanticAction(Card * card, CardList * next);
Game * GameSemanticAction(char * name, PlayerRange * players, const int handSize, CardList * deck, const int winCondition);

#endif