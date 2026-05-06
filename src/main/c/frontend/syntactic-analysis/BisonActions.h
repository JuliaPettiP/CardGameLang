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

/* Win conditions */
WinCondition * WinEmptyHandSemanticAction();
WinCondition * WinPointsSemanticAction(int points);

/* Turn */
TurnAction * TurnActionSemanticAction(TurnActionType type, char * name, int count);
TurnActionList * TurnActionListSemanticAction(TurnAction * action, TurnActionList * next);
Turn * TurnSimpleSemanticAction(TurnActionList * statements);
Turn * TurnConditionalSemanticAction(TurnActionList * ifBlock, TurnActionList * elseBlock);

/* Play rules */
PlayRule * PlayRuleSemanticAction(PlayRulePermission permission, char * subject, PlayConditionType condition, char * conditionTarget);
PlayRuleList * PlayRuleListSemanticAction(PlayRule * rule, PlayRuleList * next);

/* Game rules (P3) */
RuleStatement * RuleStatementSemanticAction(char * action, int count);
RuleStatementList * RuleStatementListSemanticAction(RuleStatement * statement, RuleStatementList * next);
GameRule * GameRuleSemanticAction(char * triggerCard, RuleStatementList * body);
GameRuleList * GameRuleListSemanticAction(GameRule * rule, GameRuleList * next);

/* Card attributes (P1) */
ColorList * ColorListSemanticAction(char * color, ColorList * next);
CardAttribute * CardColorListSemanticAction(ColorList * colors);
CardAttribute * CardColorSingleSemanticAction(char * color);
CardAttribute * CardValueSemanticAction(int min, int max);
CardAttribute * CardPointsSemanticAction(int min, int max);
CardAttribute * CardEffectSemanticAction(char * effectName, int count);
CardAttributeList * CardAttributeListSemanticAction(CardAttribute * attribute, CardAttributeList * next);

/* Declared actions (P2) */
ActionNameList * ActionNameListSemanticAction(char * name, ActionNameList * next);

/* Game tree nodes */
Program * GameProgramSemanticAction(Game * game);
PlayerRange * PlayerRangeSemanticAction(const int min, const int max);
Card * CardSemanticAction(char * name, CardAttributeList * attributes);
CardList * CardListSemanticAction(Card * card, CardList * next);
Game * GameSemanticAction(char * name, PlayerRange * players, const int handSize, CardList * deck, PlayRuleList * playRules, GameRuleList * rules, Turn * turn, ActionNameList * declaredActions, WinCondition * winCondition);

#endif