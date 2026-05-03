#include "BisonActions.h"

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

void _shutdownBisonActionsModule() {
    if (_logger != NULL) {
        logDebugging(_logger, "Destroying module: BisonActions...");
        destroyLogger(_logger);
        _logger = NULL;
    }
    _compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
    _compilerState = compilerState;
    _logger = createLogger("BisonActions");
    return _shutdownBisonActionsModule;
}

static void _logSyntacticAnalyzerAction(const char * functionName) {
    logDebugging(_logger, "%s", functionName);
}

PlayerRange * PlayerRangeSemanticAction(const int min, const int max) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    PlayerRange * range = calloc(1, sizeof(PlayerRange));
    range->min = min;
    range->max = max;
    return range;
}

Card * CardSemanticAction(char * name) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Card * card = calloc(1, sizeof(Card));
    card->name = name;
    return card;
}

CardList * CardListSemanticAction(Card * card, CardList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    CardList * list = calloc(1, sizeof(CardList));
    list->card = card;
    list->next = next;
    return list;
}

Game * GameSemanticAction(char * name, PlayerRange * players, const int handSize, CardList * deck, const int winCondition) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Game * game = calloc(1, sizeof(Game));
    game->name = name; 
    game->players = players;
    game->handSize = handSize;
    game->deck = deck;
    game->winCondition = winCondition;
    return game;
}

Program * GameProgramSemanticAction(Game * game) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program * program = calloc(1, sizeof(Program));
    program->game = game;
    _compilerState->abstractSyntaxtTree = program;
    return program;
}