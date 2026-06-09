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
        /* All three list pointers are always freed — unused ones are NULL */
        destroyTurnActionList(turn->statements);
        destroyTurnActionList(turn->ifBlock);
        destroyTurnActionList(turn->elseBlock);
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

void destroyColorList(ColorList * list) {
    if (list != NULL) {
        if (list->color != NULL) free(list->color);
        destroyColorList(list->next);
        free(list);
    }
}

void destroyCardAttribute(CardAttribute * attribute) {
    if (attribute != NULL) {
        destroyColorList(attribute->colors);
        if (attribute->effectName != NULL) free(attribute->effectName);
        free(attribute);
    }
}

void destroyCardAttributeList(CardAttributeList * list) {
    if (list != NULL) {
        destroyCardAttribute(list->attribute);
        destroyCardAttributeList(list->next);
        free(list);
    }
}

void destroyActionNameList(ActionNameList * list) {
    if (list != NULL) {
        if (list->name != NULL) free(list->name);
        destroyActionNameList(list->next);
        free(list);
    }
}

void destroyRuleStatement(RuleStatement * statement) {
    if (statement != NULL) {
        if (statement->action != NULL) free(statement->action);
        free(statement);
    }
}

void destroyRuleStatementList(RuleStatementList * list) {
    if (list != NULL) {
        destroyRuleStatement(list->statement);
        destroyRuleStatementList(list->next);
        free(list);
    }
}

void destroyGameRule(GameRule * rule) {
    if (rule != NULL) {
        if (rule->triggerCard != NULL) free(rule->triggerCard);
        destroyRuleStatementList(rule->body);
        free(rule);
    }
}

void destroyGameRuleList(GameRuleList * list) {
    if (list != NULL) {
        destroyGameRule(list->rule);
        destroyGameRuleList(list->next);
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
        destroyCardAttributeList(card->attributes);
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
        destroyGameRuleList(game->rules);
        destroyTurn(game->turn);
        destroyActionNameList(game->declaredActions);
        destroyWinCondition(game->winCondition);
        free(game);
    }
}

void destroyGameList(GameList * list) {
    if (list != NULL) {
        destroyGame(list->game);
        destroyGameList(list->next);
        free(list);
    }
}

void destroyProgram(Program * program) {
    if (program != NULL) {
        destroyGameList(program->games);
        free(program);
    }
}