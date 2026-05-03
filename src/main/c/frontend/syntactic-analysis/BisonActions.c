#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
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

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

PlayerRange * PlayerRangeSemanticAction(const int min, const int max) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    PlayerRange * range = calloc(1, sizeof(PlayerRange));
    range->min = min;
    range->max = max;
    return range;
}

Game * GameSemanticAction(char * name, PlayerRange * players, const int handSize) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Game * game = calloc(1, sizeof(Game));
    game->name = name; // Não faças strdup aqui, o Flex já o fez!
    game->players = players;
    game->handSize = handSize;
    return game;
}

Program * GameProgramSemanticAction(Game * game) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program * program = calloc(1, sizeof(Program));
    program->game = game;
    _compilerState->abstractSyntaxtTree = program;
    return program;
}