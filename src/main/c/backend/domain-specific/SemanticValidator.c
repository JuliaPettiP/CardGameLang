#include "SemanticValidator.h"
#include "../../support/logging/Logger.h"
#include <stdbool.h>
#include <string.h>

static Logger * _logger = NULL;

/* Checks whether an action name was declared in the actions section. */
static bool actionExists(ActionNameList * actions, const char * name) {
	while (actions != NULL) {
		if (strcmp(actions->name, name) == 0) return true;
		actions = actions->next;
	}
	return false;
}

/* Checks whether a card name exists in the deck. */
static bool cardExists(CardList * deck, const char * name) {
	while (deck != NULL) {
		if (strcmp(deck->card->name, name) == 0) return true;
		deck = deck->next;
	}
	return false;
}

/* Validates that the players range exists and is coherent */
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

/* Validates that every action used inside a rule body was declared in the actions section. */
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

/* Validates game rules: referenced trigger cards must exist and rule actions must be declared. */
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

/* Validates that every action used in a turn block was declared in the actions section. */
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

/* Validates the complete turn section, including simple and conditional turns. */
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

/* Validates play_rule references: every named card used in the rule must exist in the deck. */
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

/* Validates that action names are not repeated in the actions section. */
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

/* Validates declared actions: names cannot be empty and cannot be duplicated. */
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

/* Validates that card names are not repeated inside the deck. */
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

/* Validates the deck: it cannot be empty, card names cannot be empty, and cards cannot be duplicated. */
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

/* Validates the win condition: it must exist and reach_points must be greater than zero. */
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

/* Validates that the initial hand size is greater than zero. */
static CompilationStatus validateHandSize(Game * game) {
	if (game->handSize <= 0) {
		logError(_logger, "Invalid hand size: %d", game->handSize);
		return FAILED;
	}

	return SUCCEEDED;
}

/* Runs every semantic validation for a single game. */
static CompilationStatus validateGame(Game * game) {
	if (validatePlayers(game) == FAILED ||
		validateDeck(game) == FAILED ||
		validateActions(game) == FAILED ||
		validateWinCondition(game) == FAILED ||
		validateHandSize(game) == FAILED ||
		validatePlayRules(game) == FAILED ||
		validateRules(game) == FAILED ||
		validateTurn(game) == FAILED) {
		return FAILED;
	}

	return SUCCEEDED;
}

/* Entry point for semantic validation. Validates every game contained in the program. */
CompilationStatus validateSemantics(Program * program) {
	_logger = createLogger("SemanticValidator");

	if (program == NULL || program->games == NULL) {
		logError(_logger, "Missing program.");
		destroyLogger(_logger);
		return FAILED;
	}

	CompilationStatus status = SUCCEEDED;

	for (GameList * games = program->games; games != NULL; games = games->next) {
		if (validateGame(games->game) == FAILED) {
			status = FAILED;
			break;
		}
	}

	destroyLogger(_logger);
	_logger = NULL;
	return status;
}