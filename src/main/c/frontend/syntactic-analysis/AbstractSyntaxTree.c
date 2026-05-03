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
        logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
        free(range);
    }
}

void destroyGame(Game * game) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (game != NULL) {
        destroyPlayerRange(game->players); 
        if (game->name != NULL) free(game->name); 
        free(game);
    }
}

void destroyProgram(Program * program) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (program != NULL) {
        destroyGame(program->game);
        free(program);
    }
}
