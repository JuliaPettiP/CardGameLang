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

/* ------------------------------------------------------------------ */
/*  Win condition                                                       */
/* ------------------------------------------------------------------ */

WinCondition * WinEmptyHandSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    WinCondition * win = calloc(1, sizeof(WinCondition));
    win->type = WIN_EMPTY_HAND;
    win->points = 0;
    return win;
}

WinCondition * WinPointsSemanticAction(int points) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    WinCondition * win = calloc(1, sizeof(WinCondition));
    win->type = WIN_REACH_POINTS;
    win->points = points;
    return win;
}

/* ------------------------------------------------------------------ */
/*  Turn / Actions                                                      */
/* ------------------------------------------------------------------ */

TurnAction * TurnActionSemanticAction(TurnActionType type, char * name) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TurnAction * action = calloc(1, sizeof(TurnAction));
    action->type = type;
    action->name = name;  /* ownership of the strdup'd string transfers here */
    return action;
}

TurnActionList * TurnActionListSemanticAction(TurnAction * action, TurnActionList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TurnActionList * list = calloc(1, sizeof(TurnActionList));
    list->action = action;
    list->next = next;
    return list;
}

Turn * TurnSemanticAction(TurnActionList * actions) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Turn * turn = calloc(1, sizeof(Turn));
    turn->actions = actions;
    return turn;
}

/* ------------------------------------------------------------------ */
/*  Play rules                                                          */
/* ------------------------------------------------------------------ */

PlayRule * PlayRuleSemanticAction(PlayRulePermission permission, char * subject, PlayConditionType condition, char * conditionTarget) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    PlayRule * rule = calloc(1, sizeof(PlayRule));
    rule->permission = permission;
    rule->subject = subject;               /* NULL for keyword conditions  */
    rule->condition = condition;
    rule->conditionTarget = conditionTarget; /* NULL when not PLAYED_CARD  */
    return rule;
}

PlayRuleList * PlayRuleListSemanticAction(PlayRule * rule, PlayRuleList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    PlayRuleList * list = calloc(1, sizeof(PlayRuleList));
    list->rule = rule;
    list->next = next;
    return list;
}

/* ------------------------------------------------------------------ */
/*  Game tree nodes                                                     */
/* ------------------------------------------------------------------ */

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

Game * GameSemanticAction(char * name, PlayerRange * players, const int handSize, CardList * deck, PlayRuleList * playRules, Turn * turn, WinCondition * winCondition) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Game * game = calloc(1, sizeof(Game));
    game->name = name;
    game->players = players;
    game->handSize = handSize;
    game->deck = deck;
    game->playRules = playRules;    /* may be NULL */
    game->turn = turn;              /* may be NULL */
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