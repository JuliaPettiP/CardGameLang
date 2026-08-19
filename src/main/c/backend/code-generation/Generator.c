#include "Generator.h"
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Configuration                                                       */
/* ------------------------------------------------------------------ */

#define MAX_PLAYERS 8
#define MAX_TURNS   100   /* hard cap so the simulation always terminates */

/* ------------------------------------------------------------------ */
/*  Game summary (static description of the parsed game)                */
/* ------------------------------------------------------------------ */

static void printCards(CardList * cards) {
	while (cards != NULL) {
		printf("- %s\n", cards->card->name);
		cards = cards->next;
	}
}

static void printActions(ActionNameList * actions) {
	while (actions != NULL) {
		printf("- %s\n", actions->name);
		actions = actions->next;
	}
}

static void printSummary(Game * game) {
	printf("\n=== GAME SUMMARY ===\n");
	printf("Game: %s\n", game->name);
	printf("Players: %d..%d\n", game->players->min, game->players->max);
	printf("Initial hand: %d\n", game->handSize);

	printf("\nCards:\n");
	printCards(game->deck);

	printf("\nActions:\n");
	printActions(game->declaredActions);

	printf("\nWin condition:\n");
	if (game->winCondition->type == WIN_EMPTY_HAND) {
		printf("- empty_hand\n");
	}
	else {
		printf("- reach_points %d\n", game->winCondition->points);
	}
}

/* ------------------------------------------------------------------ */
/*  Textual simulation of a match                                       */
/* ------------------------------------------------------------------ */

typedef struct {
	int hand;     /* number of cards currently in hand */
	int points;   /* accumulated points                */
} PlayerState;

/* Deterministic pseudo-random generator so the simulation is reproducible. */
static unsigned int _rngState = 0u;

static void rngReset(void) {
	_rngState = 0x12345678u;
}

static int rngBetween(int low, int high) {
	if (high <= low) {
		return low;
	}
	_rngState = _rngState * 1103515245u + 12345u;
	return low + (int) ((_rngState >> 16) % (unsigned int) (high - low + 1));
}

static int countCards(CardList * deck) {
	int count = 0;
	while (deck != NULL) {
		count++;
		deck = deck->next;
	}
	return count;
}

static Card * cardByIndex(CardList * deck, int index) {
	while (index > 0 && deck != NULL) {
		deck = deck->next;
		index--;
	}
	return (deck != NULL) ? deck->card : NULL;
}

/* Returns the point value granted by a card (0 when it has no points attribute). */
static int cardPoints(Card * card) {
	CardAttributeList * attributes = card->attributes;
	while (attributes != NULL) {
		if (attributes->attribute->type == CARD_ATTR_POINTS) {
			return rngBetween(attributes->attribute->rangeMin,
				attributes->attribute->rangeMax);
		}
		attributes = attributes->next;
	}
	return 0;
}

static GameRule * findRule(Game * game, const char * cardName) {
	GameRuleList * rules = game->rules;
	while (rules != NULL) {
		if (strcmp(rules->rule->triggerCard, cardName) == 0) {
			return rules->rule;
		}
		rules = rules->next;
	}
	return NULL;
}

/*
 * Applies the body of the rule triggered by playing `cardName`, if any.
 * Recognised effects: next_player <draw N>, draw N, skip_next_player,
 * extra_turn. Any other effect is narrated without changing the state.
 */
static void applyTriggeredRule(Game * game, PlayerState * players, int numPlayers,
	int active, const char * cardName, int * skipNext, int * extraTurn) {
	GameRule * rule = findRule(game, cardName);
	if (rule == NULL) {
		return;
	}

	int target = active;   /* effects apply to the active player by default */
	RuleStatementList * statements = rule->body;
	while (statements != NULL) {
		RuleStatement * statement = statements->statement;
		const char * action = statement->action;
		int count = statement->count > 0 ? statement->count : 1;

		if (strcmp(action, "next_player") == 0) {
			target = (active + 1) % numPlayers;
		}
		else if (strcmp(action, "draw") == 0) {
			players[target].hand += count;
			printf("        rule: player %d draws %d (played %s)\n",
				target + 1, count, cardName);
		}
		else if (strcmp(action, "skip_next_player") == 0) {
			*skipNext = 1;
			printf("        rule: next player is skipped (played %s)\n", cardName);
		}
		else if (strcmp(action, "extra_turn") == 0) {
			*extraTurn = 1;
			printf("        rule: player %d takes an extra turn (played %s)\n",
				active + 1, cardName);
		}
		else {
			printf("        rule: effect '%s' (played %s)\n", action, cardName);
		}

		statements = statements->next;
	}
}

/*
 * Plays up to `count` cards for the active player. Updates hand and points,
 * applies any triggered rule and checks the win condition after every card.
 * Returns the winner index (>= 0) as soon as the game is won, or -1 otherwise.
 */
static int playCards(Game * game, PlayerState * players, int numPlayers,
	int active, int count, int * deckCursor, int numCards,
	int * skipNext, int * extraTurn) {
	WinCondition * win = game->winCondition;

	for (int played = 0; played < count; played++) {
		if (players[active].hand <= 0) {
			printf("        player %d has no cards to play\n", active + 1);
			break;
		}

		Card * card = cardByIndex(game->deck, *deckCursor % numCards);
		*deckCursor = *deckCursor + 1;

		players[active].hand--;
		int gained = cardPoints(card);
		players[active].points += gained;

		if (gained > 0) {
			printf("        player %d plays %s (+%d points, total %d, hand %d)\n",
				active + 1, card->name, gained, players[active].points,
				players[active].hand);
		}
		else {
			printf("        player %d plays %s (hand %d)\n",
				active + 1, card->name, players[active].hand);
		}

		applyTriggeredRule(game, players, numPlayers, active, card->name,
			skipNext, extraTurn);

		if (win->type == WIN_EMPTY_HAND && players[active].hand <= 0) {
			return active;
		}
		if (win->type == WIN_REACH_POINTS && players[active].points >= win->points) {
			return active;
		}
	}

	return -1;
}

/*
 * Decides whether an optional ("may") action should be taken. We play whenever
 * possible (progress towards both win conditions) and only draw when the hand
 * is empty, so the simulation makes progress and terminates.
 */
static int shouldTakeOptional(const char * action, PlayerState * player) {
	if (strcmp(action, "draw") == 0) {
		return player->hand <= 0;
	}
	return 1;
}

/*
 * Executes a single turn action for the active player. Returns the winner
 * index when the game is won during the action, or -1 otherwise.
 */
static int runTurnAction(Game * game, PlayerState * players, int numPlayers,
	int active, TurnAction * action, int * deckCursor, int numCards,
	int * skipNext, int * extraTurn) {
	if (action->type == TURN_ACTION_MAY &&
		!shouldTakeOptional(action->name, &players[active])) {
		printf("        player %d skips optional '%s'\n", active + 1, action->name);
		return -1;
	}

	int count = action->count > 0 ? action->count : 1;

	if (strcmp(action->name, "play") == 0) {
		return playCards(game, players, numPlayers, active, count,
			deckCursor, numCards, skipNext, extraTurn);
	}
	if (strcmp(action->name, "draw") == 0) {
		players[active].hand += count;
		printf("        player %d draws %d (hand %d)\n",
			active + 1, count, players[active].hand);
		return -1;
	}

	printf("        player %d performs '%s'\n", active + 1, action->name);
	return -1;
}

/* Runs every action of a statement list; returns the winner index or -1. */
static int runActionList(Game * game, PlayerState * players, int numPlayers,
	int active, TurnActionList * list, int * deckCursor, int numCards,
	int * skipNext, int * extraTurn) {
	while (list != NULL) {
		int winner = runTurnAction(game, players, numPlayers, active,
			list->action, deckCursor, numCards, skipNext, extraTurn);
		if (winner >= 0) {
			return winner;
		}
		list = list->next;
	}
	return -1;
}

/* Resolves which statement list applies this turn and runs it. */
static int runTurn(Game * game, PlayerState * players, int numPlayers,
	int active, int * deckCursor, int numCards, int * skipNext, int * extraTurn) {
	Turn * turn = game->turn;

	/* No turn section: default behaviour is to play a single card. */
	if (turn == NULL) {
		return playCards(game, players, numPlayers, active, 1,
			deckCursor, numCards, skipNext, extraTurn);
	}

	if (turn->type == TURN_SIMPLE) {
		return runActionList(game, players, numPlayers, active,
			turn->statements, deckCursor, numCards, skipNext, extraTurn);
	}

	/* TURN_CONDITIONAL: "cannot_play" is true when the hand is empty. */
	int canPlay = players[active].hand > 0;
	if (!canPlay) {
		printf("        player %d cannot play\n", active + 1);
		return runActionList(game, players, numPlayers, active,
			turn->ifBlock, deckCursor, numCards, skipNext, extraTurn);
	}
	return runActionList(game, players, numPlayers, active,
		turn->elseBlock, deckCursor, numCards, skipNext, extraTurn);
}

static void runSimulation(Game * game) {
	int numCards = countCards(game->deck);
	if (numCards == 0) {
		return;   /* deck is validated to be non-empty, but stay safe */
	}

	int numPlayers = game->players->min;
	if (numPlayers < 1) {
		numPlayers = 1;
	}
	if (numPlayers > MAX_PLAYERS) {
		numPlayers = MAX_PLAYERS;
	}

	rngReset();

	PlayerState players[MAX_PLAYERS];
	for (int i = 0; i < numPlayers; i++) {
		players[i].hand = game->handSize;
		players[i].points = 0;
	}

	printf("\n=== SIMULATION ===\n");
	printf("Players: %d, initial hand: %d each\n", numPlayers, game->handSize);

	int deckCursor = 0;
	int active = 0;
	int skipNext = 0;
	int winner = -1;
	int turnNo = 0;

	while (turnNo < MAX_TURNS && winner < 0) {
		turnNo++;
		int extraTurn = 0;

		printf("\nTurn %d - player %d (hand %d, points %d):\n",
			turnNo, active + 1, players[active].hand, players[active].points);

		winner = runTurn(game, players, numPlayers, active,
			&deckCursor, numCards, &skipNext, &extraTurn);
		if (winner >= 0) {
			break;
		}

		if (extraTurn) {
			continue;   /* same player plays again */
		}

		active = (active + 1) % numPlayers;
		if (skipNext) {
			printf("        player %d is skipped\n", active + 1);
			active = (active + 1) % numPlayers;
			skipNext = 0;
		}
	}

	printf("\n=== RESULT ===\n");
	if (winner >= 0) {
		if (game->winCondition->type == WIN_EMPTY_HAND) {
			printf("Player %d wins by emptying their hand (turn %d).\n",
				winner + 1, turnNo);
		}
		else {
			printf("Player %d wins by reaching %d points (turn %d).\n",
				winner + 1, players[winner].points, turnNo);
		}
	}
	else {
		printf("No winner after %d turns (simulation cap reached).\n", MAX_TURNS);
	}
}

/* ------------------------------------------------------------------ */
/*  Public entry point                                                  */
/* ------------------------------------------------------------------ */

void generateGameSummary(Program * program) {
	for (GameList * games = program->games; games != NULL; games = games->next) {
		printSummary(games->game);
		runSimulation(games->game);
	}
}
