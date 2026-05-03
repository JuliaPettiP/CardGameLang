#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
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
void destroyPlayerRange(PlayerRange * range) {
    if (range != NULL) {
        free(range);
    }
}

void destroyCard(Card * card) {
    if (card != NULL) {
        if (card->name != NULL) free(card->name); // Liberta a string do nome
        free(card);
    }
}

void destroyCardList(CardList * list) {
    if (list != NULL) {
        destroyCard(list->card);     // Liberta a carta deste nó
        destroyCardList(list->next); // Chama a próxima (recursividade)
        free(list);                  // Liberta o próprio nó
    }
}

void destroyGame(Game * game) {
    if (game != NULL) {
        destroyPlayerRange(game->players);
        destroyCardList(game->deck); // Limpa o baralho inteiro
        if (game->name != NULL) free(game->name);
        free(game);
    }
}

void destroyProgram(Program * program) {
    if (program != NULL) {
        destroyGame(program->game);
        free(program);
    }
}