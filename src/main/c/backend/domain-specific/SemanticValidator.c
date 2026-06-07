#include "SemanticValidator.h"
#include "../../support/logging/Logger.h"
#include <stdbool.h>
#include <string.h>

static Logger * _logger = NULL;

static bool actionExists(ActionNameList * actions, const char * name) {
	while (actions != NULL) {
		if (strcmp(actions->name, name) == 0) return true;
		actions = actions->next;
	}
	return false;
}

static bool cardExists(CardList * deck, const char * name) {
	while (deck != NULL) {
		if (strcmp(deck->card->name, name) == 0) return true;
		deck = deck->next;
	}
	return false;
}

static CompilationStatus validatePlayers(Game * game) {
	if (game->players == NULL ||
		game->players->min < 1 ||
		game->players->max < 1 ||
		game->players->min > game->players->max) {
		logError(_logger, "Invalid players range.");
		return FAILED;
	}
	return SUCCEEDED;
}

static CompilationStatus validateRuleStatements(Game * game, RuleStatementList * statements) {
	while (statements != NULL) {
		if (!actionExists(game->declaredActions, statements->statement->action)) {
			logError(_logger, "Undeclared action in rules: %s", statements->statement->action);
			return FAILED;
		}
		statements = statements->next;
	}
	return SUCCEEDED;
}

static CompilationStatus validateRules(Game * game) {
	GameRuleList * rules = game->rules;

	while (rules != NULL) {
		if (!cardExists(game->deck, rules->rule->triggerCard)) {
			logError(_logger, "Rule references undefined card: %s", rules->rule->triggerCard);
			return FAILED;
		}

		if (validateRuleStatements(game, rules->rule->body) == FAILED) {
			return FAILED;
		}

		rules = rules->next;
	}

	return SUCCEEDED;
}

static CompilationStatus validateTurnActions(Game * game, TurnActionList * actions) {
	while (actions != NULL) {
		if (!actionExists(game->declaredActions, actions->action->name)) {
			logError(_logger, "Undeclared action in turn: %s", actions->action->name);
			return FAILED;
		}
		actions = actions->next;
	}
	return SUCCEEDED;
}

static CompilationStatus validateTurn(Game * game) {
	if (game->turn == NULL) return SUCCEEDED;

	if (game->turn->type == TURN_SIMPLE) {
		return validateTurnActions(game, game->turn->statements);
	}

	if (validateTurnActions(game, game->turn->ifBlock) == FAILED) {
		return FAILED;
	}

	return validateTurnActions(game, game->turn->elseBlock);
}

static CompilationStatus validatePlayRules(Game * game) {
	PlayRuleList * rules = game->playRules;

	while (rules != NULL) {
		PlayRule * rule = rules->rule;

		if (rule->subject != NULL && !cardExists(game->deck, rule->subject)) {
			logError(_logger, "Play rule references undefined card: %s", rule->subject);
			return FAILED;
		}

		if (rule->conditionTarget != NULL && !cardExists(game->deck, rule->conditionTarget)) {
			logError(_logger, "Play rule references undefined card: %s", rule->conditionTarget);
			return FAILED;
		}

		rules = rules->next;
	}

	return SUCCEEDED;
}

static CompilationStatus validateUniqueActions(Game * game) {
	ActionNameList * current = game->declaredActions;

	while (current != NULL) {
		ActionNameList * other = current->next;

		while (other != NULL) {
			if (strcmp(current->name, other->name) == 0) {
				logError(_logger, "Duplicated action name: %s", current->name);
				return FAILED;
			}

			other = other->next;
		}

		current = current->next;
	}

	return SUCCEEDED;
}

static CompilationStatus validateActions(Game * game) {
	ActionNameList * actions = game->declaredActions;

	while (actions != NULL) {
		if (strlen(actions->name) == 0) {
			logError(_logger, "Action name cannot be empty.");
			return FAILED;
		}
		actions = actions->next;
	}

	return validateUniqueActions(game);
}

static CompilationStatus validateUniqueCards(Game * game) {
	CardList * current = game->deck;

	while (current != NULL) {
		CardList * other = current->next;

		while (other != NULL) {
			if (strcmp(current->card->name, other->card->name) == 0) {
				logError(_logger, "Duplicated card name in deck: %s", current->card->name);
				return FAILED;
			}

			other = other->next;
		}

		current = current->next;
	}

	return SUCCEEDED;
}

static CompilationStatus validateDeck(Game * game) {
	CardList * deck = game->deck;

	if (deck == NULL) {
		logError(_logger, "Deck cannot be empty.");
		return FAILED;
	}

	while (deck != NULL) {
		if (strlen(deck->card->name) == 0) {
			logError(_logger, "Card name cannot be empty.");
			return FAILED;
		}
		deck = deck->next;
	}

	return validateUniqueCards(game);
}

static CompilationStatus validateWinCondition(Game * game) {
	if (game->winCondition == NULL) {
		logError(_logger, "Missing win condition.");
		return FAILED;
	}

	if (game->winCondition->type == WIN_REACH_POINTS &&
		game->winCondition->points <= 0) {
		logError(_logger, "Invalid reach_points target: %d", game->winCondition->points);
		return FAILED;
	}

	return SUCCEEDED;
}

static CompilationStatus validateHandSize(Game * game) {
	if (game->handSize <= 0) {
		logError(_logger, "Invalid hand size: %d", game->handSize);
		return FAILED;
	}

	return SUCCEEDED;
}

CompilationStatus validateSemantics(Program * program) {
	_logger = createLogger("SemanticValidator");

	if (program == NULL || program->game == NULL) {
		logError(_logger, "Missing program.");
		destroyLogger(_logger);
		return FAILED;
	}

	Game * game = program->game;

	CompilationStatus status = SUCCEEDED;

	if (validatePlayers(game) == FAILED ||
		validateDeck(game) == FAILED ||
		validateActions(game) == FAILED ||
		validateWinCondition(game) == FAILED ||
		validateHandSize(game) == FAILED ||
		validatePlayRules(game) == FAILED ||
		validateRules(game) == FAILED ||
		validateTurn(game) == FAILED) {
		status = FAILED;
	}

	destroyLogger(_logger);
	_logger = NULL;
	return status;
}

/* MODULE INTERNAL STATE */

//static Logger * _logger = NULL;

/** Shutdown module's internal state. */
/*void _shutdownCalculatorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Calculator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeCalculatorModule() {
	_logger = createLogger("Calculator");
	return _shutdownCalculatorModule;
}

/** PRIVATE FUNCTIONS */

/*static BinaryOperator _expressionTypeToBinaryOperator(const ExpressionType type);
static ComputationResult _invalidBinaryOperator(const int x, const int y);
static ComputationResult _invalidComputation();

/**
 * Converts and expression type to the proper binary operator. If that's not
 * possible, returns a binary operator that always returns an invalid
 * computation result.
 */
/*static BinaryOperator _expressionTypeToBinaryOperator(const ExpressionType type) {
	switch (type) {
		case ADDITION: return add;
		case DIVISION: return divide;
		case MULTIPLICATION: return multiply;
		case SUBTRACTION: return subtract;
		default:
			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
			return _invalidBinaryOperator;
	}
}

/**
 * A binary operator that always returns an invalid computation result.
 */
/*static ComputationResult _invalidBinaryOperator(const int x, const int y) {
	return _invalidComputation();
}

/**
 * A computation that always returns an invalid result.
 */
/*static ComputationResult _invalidComputation() {
	ComputationResult computationResult = {
		.succeeded = false,
		.value = 0
	};
	return computationResult;
}

/** PUBLIC FUNCTIONS */

/*ComputationResult add(const int leftAddend, const int rightAddend) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = leftAddend + rightAddend
	};
	return computationResult;
}

ComputationResult divide(const int dividend, const int divisor) {
	const int sign = dividend < 0 ? -1 : +1;
	const bool divisionByZero = divisor == 0 ? true : false;
	if (divisionByZero) {
		logError(_logger, "The divisor cannot be zero (the computation was %d/%d).", dividend, divisor);
	}
	ComputationResult computationResult = {
		.succeeded = divisionByZero ? false : true,
		.value = divisionByZero ? (sign * INT_MAX) : (dividend / divisor)
	};
	return computationResult;
}

ComputationResult multiply(const int multiplicand, const int multiplier) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = multiplicand * multiplier
	};
	return computationResult;
}

ComputationResult subtract(const int minuend, const int subtract) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = minuend - subtract
	};
	return computationResult;
}

ComputationResult computeConstant(Constant * constant) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = constant->value
	};
	return computationResult;
}

ComputationResult computeExpression(Expression * expression) {
	switch (expression->type) {
		case ADDITION:
		case DIVISION:
		case MULTIPLICATION:
		case SUBTRACTION:
			ComputationResult leftResult = computeExpression(expression->leftExpression);
			ComputationResult rightResult = computeExpression(expression->rightExpression);
			if (leftResult.succeeded && rightResult.succeeded) {
				BinaryOperator binaryOperator = _expressionTypeToBinaryOperator(expression->type);
				return binaryOperator(leftResult.value, rightResult.value);
			}
			else {
				return _invalidComputation();
			}
		case FACTOR:
			return computeFactor(expression->factor);
		default:
			return _invalidComputation();
	}
}

ComputationResult computeFactor(Factor * factor) {
	switch (factor->type) {
		case CONSTANT:
			return computeConstant(factor->constant);
		case EXPRESSION:
			return computeExpression(factor->expression);
		default:
			return _invalidComputation();
	}
}

ComputationResult executeCalculator(CompilerState * compilerState) {
	Program * program = compilerState->abstractSyntaxtTree;
	return computeExpression(program->expression);
}*/
