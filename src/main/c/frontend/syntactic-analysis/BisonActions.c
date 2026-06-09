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
/*  Game rules (P3)                                                    */
/* ------------------------------------------------------------------ */

RuleStatement * RuleStatementSemanticAction(char * action, int count) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    RuleStatement * stmt = calloc(1, sizeof(RuleStatement));
    stmt->action = action;
    stmt->count = count;
    return stmt;
}

RuleStatementList * RuleStatementListSemanticAction(RuleStatement * statement, RuleStatementList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    RuleStatementList * list = calloc(1, sizeof(RuleStatementList));
    list->statement = statement;
    list->next = next;
    return list;
}

GameRule * GameRuleSemanticAction(char * triggerCard, RuleStatementList * body) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    GameRule * rule = calloc(1, sizeof(GameRule));
    rule->triggerCard = triggerCard;
    rule->body = body;
    return rule;
}

GameRuleList * GameRuleListSemanticAction(GameRule * rule, GameRuleList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    GameRuleList * list = calloc(1, sizeof(GameRuleList));
    list->rule = rule;
    list->next = next;
    return list;
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

TurnAction * TurnActionSemanticAction(TurnActionType type, char * name, int count) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TurnAction * action = calloc(1, sizeof(TurnAction));
    action->type = type;
    action->name = name;   /* ownership of the strdup'd string transfers here */
    action->count = count; /* 0 when no integer was written                   */
    return action;
}

TurnActionList * TurnActionListSemanticAction(TurnAction * action, TurnActionList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    TurnActionList * list = calloc(1, sizeof(TurnActionList));
    list->action = action;
    list->next = next;
    return list;
}

/* turn { must play 1   may draw 1   … } */
Turn * TurnSimpleSemanticAction(TurnActionList * statements) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Turn * turn = calloc(1, sizeof(Turn));
    turn->type = TURN_SIMPLE;
    turn->statements = statements;
    turn->ifBlock = NULL;
    turn->elseBlock = NULL;
    return turn;
}

/* turn { if cannot_play { … } else { … } } */
Turn * TurnConditionalSemanticAction(TurnActionList * ifBlock, TurnActionList * elseBlock) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Turn * turn = calloc(1, sizeof(Turn));
    turn->type = TURN_CONDITIONAL;
    turn->statements = NULL;
    turn->ifBlock = ifBlock;
    turn->elseBlock = elseBlock;
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
/*  Card attributes (P1)                                               */
/* ------------------------------------------------------------------ */

ColorList * ColorListSemanticAction(char * color, ColorList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ColorList * node = calloc(1, sizeof(ColorList));
    node->color = color;
    node->next = next;
    return node;
}

CardAttribute * CardColorListSemanticAction(ColorList * colors) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    CardAttribute * attr = calloc(1, sizeof(CardAttribute));
    attr->type = CARD_ATTR_COLOR;
    attr->colors = colors;
    return attr;
}

CardAttribute * CardColorSingleSemanticAction(char * color) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    /* Wrap the single color in a one-element ColorList for uniform access */
    ColorList * node = calloc(1, sizeof(ColorList));
    node->color = color;
    node->next = NULL;
    CardAttribute * attr = calloc(1, sizeof(CardAttribute));
    attr->type = CARD_ATTR_COLOR;
    attr->colors = node;
    return attr;
}

CardAttribute * CardValueSemanticAction(int min, int max) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    CardAttribute * attr = calloc(1, sizeof(CardAttribute));
    attr->type = CARD_ATTR_VALUE;
    attr->rangeMin = min;
    attr->rangeMax = max;
    return attr;
}

CardAttribute * CardPointsSemanticAction(int min, int max) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    CardAttribute * attr = calloc(1, sizeof(CardAttribute));
    attr->type = CARD_ATTR_POINTS;
    attr->rangeMin = min;
    attr->rangeMax = max;
    return attr;
}

CardAttribute * CardEffectSemanticAction(char * effectName, int count) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    CardAttribute * attr = calloc(1, sizeof(CardAttribute));
    attr->type = CARD_ATTR_EFFECT;
    attr->effectName = effectName;
    attr->effectCount = count;
    return attr;
}

CardAttributeList * CardAttributeListSemanticAction(CardAttribute * attribute, CardAttributeList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    CardAttributeList * list = calloc(1, sizeof(CardAttributeList));
    list->attribute = attribute;
    list->next = next;
    return list;
}

/* ------------------------------------------------------------------ */
/*  Declared actions (P2)                                              */
/* ------------------------------------------------------------------ */

ActionNameList * ActionNameListSemanticAction(char * name, ActionNameList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ActionNameList * node = calloc(1, sizeof(ActionNameList));
    node->name = name;
    node->next = next;
    return node;
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

Card * CardSemanticAction(char * name, CardAttributeList * attributes) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Card * card = calloc(1, sizeof(Card));
    card->name = name;
    card->attributes = attributes;   /* NULL when card body is empty */
    return card;
}

CardList * CardListSemanticAction(Card * card, CardList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    CardList * list = calloc(1, sizeof(CardList));
    list->card = card;
    list->next = next;
    return list;
}

Game * GameSemanticAction(char * name, PlayerRange * players, const int handSize, CardList * deck, PlayRuleList * playRules, GameRuleList * rules, Turn * turn, ActionNameList * declaredActions, WinCondition * winCondition) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Game * game = calloc(1, sizeof(Game));
    game->name = name;
    game->players = players;
    game->handSize = handSize;
    game->deck = deck;
    game->playRules = playRules;
    game->rules = rules;                 /* may be NULL */
    game->turn = turn;                   /* may be NULL */
    game->declaredActions = declaredActions;
    game->winCondition = winCondition;
    return game;
}

GameList * GameListSemanticAction(Game * game, GameList * next) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    GameList * list = calloc(1, sizeof(GameList));
    list->game = game;
    list->next = next;
    return list;
}

Program * GameProgramSemanticAction(GameList * games) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program * program = calloc(1, sizeof(Program));
    program->games = games;
    _compilerState->abstractSyntaxtTree = program;
    return program;
}