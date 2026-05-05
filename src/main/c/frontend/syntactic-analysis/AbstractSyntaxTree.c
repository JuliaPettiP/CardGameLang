#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */
static Logger * _logger = NULL;

void _shutdownAbstractSyntaxTreeModule() {
    if (_logger != NULL) {
        logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
    _logger = createLogger("AbstractSyntaxTree");
    return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyWinCondition(WinCondition * winCondition) {
    if (winCondition != NULL) {
        free(winCondition);
    }
}

void destroyTurnAction(TurnAction * action) {
    if (action != NULL) {
        if (action->name != NULL) free(action->name);
        free(action);
    }
}

void destroyTurnActionList(TurnActionList * list) {
    if (list != NULL) {
        destroyTurnAction(list->action);
        destroyTurnActionList(list->next);
        free(list);
    }
}

void destroyTurn(Turn * turn) {
    if (turn != NULL) {
        destroyTurnActionList(turn->actions);
        free(turn);
    }
}

void destroyPlayRule(PlayRule * rule) {
    if (rule != NULL) {
        if (rule->subject != NULL) free(rule->subject);
        if (rule->conditionTarget != NULL) free(rule->conditionTarget);
        free(rule);
    }
}

void destroyPlayRuleList(PlayRuleList * list) {
    if (list != NULL) {
        destroyPlayRule(list->rule);
        destroyPlayRuleList(list->next);
        free(list);
    }
}

void destroyPlayerRange(PlayerRange * range) {
    if (range != NULL) {
        free(range);
    }
}

void destroyCard(Card * card) {
    if (card != NULL) {
        if (card->name != NULL) free(card->name);
        free(card);
    }
}

void destroyCardList(CardList * list) {
    if (list != NULL) {
        destroyCard(list->card);
        destroyCardList(list->next);
        free(list);
    }
}

void destroyGame(Game * game) {
    if (game != NULL) {
        if (game->name != NULL) free(game->name);
        destroyPlayerRange(game->players);
        destroyCardList(game->deck);
        destroyPlayRuleList(game->playRules);
        destroyTurn(game->turn);
        destroyWinCondition(game->winCondition);
        free(game);
    }
}

void destroyProgram(Program * program) {
    if (program != NULL) {
        destroyGame(program->game);
        free(program);
    }
}