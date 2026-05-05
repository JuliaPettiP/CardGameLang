%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

void yyerror(const YYLTYPE * location, const char * message) {}

%}

%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
    signed int integer;
    char * string;
    TokenLabel token;

    Program * program;
    Game * game;
    PlayerRange * player_range;
    Card * card;
    CardList * card_list;
    WinCondition * win_condition;
    TurnAction * turn_action;
    TurnActionList * turn_action_list;
    Turn * turn;
    PlayRule * play_rule;
    PlayRuleList * play_rule_list;
}

%destructor { destroyGame($$); } <game>
%destructor { destroyPlayerRange($$); } <player_range>
%destructor { destroyCard($$); } <card>
%destructor { destroyCardList($$); } <card_list>
%destructor { destroyWinCondition($$); } <win_condition>
%destructor { destroyTurnAction($$); } <turn_action>
%destructor { destroyTurnActionList($$); } <turn_action_list>
%destructor { destroyTurn($$); } <turn>
%destructor { destroyPlayRule($$); } <play_rule>
%destructor { destroyPlayRuleList($$); } <play_rule_list>
%destructor { free($$); } <string>

/** Terminals */
%token <integer> INTEGER
%token <string> IDENTIFIER
%token <token> GAME PLAYERS DECK CARD HAND PLAY_RULE RULES TURN ACTIONS WIN
%token <token> IF ALLOW PLAYED CANNOT_PLAY ELSE MAY MUST
%token <token> EMPTY_HAND REACH_POINTS SAME_COLOR SAME_VALUE WILD ANY_CARD
%token <token> RANGE OPEN_BRACE CLOSE_BRACE OPEN_COMMENT CLOSE_COMMENT COMMA IGNORED UNKNOWN

/** Non-terminals */
%type <program> program
%type <game> card_game
%type <player_range> players_section
%type <integer> hand_section
%type <card_list> deck_section card_list
%type <card> card_definition
%type <win_condition> win_section
%type <turn> turn_section
%type <turn_action> action_item
%type <turn_action_list> action_list
%type <play_rule_list> play_rule_section play_rule_list
%type <play_rule> play_rule_item

%%

program: card_game {
    $$ = GameProgramSemanticAction($1);
}

card_game: GAME IDENTIFIER OPEN_BRACE players_section hand_section deck_section play_rule_section turn_section win_section CLOSE_BRACE {
    $$ = GameSemanticAction($2, $4, $5, $6, $7, $8, $9);
}

players_section: PLAYERS INTEGER RANGE INTEGER {
    $$ = PlayerRangeSemanticAction($2, $4);
}
| PLAYERS INTEGER {
    $$ = PlayerRangeSemanticAction($2, $2);
}

hand_section: HAND INTEGER {
    $$ = $2;
}

deck_section: DECK OPEN_BRACE card_list CLOSE_BRACE {
    $$ = $3;
}

card_list: card_definition {
    $$ = CardListSemanticAction($1, NULL);
}
| card_definition card_list {
    $$ = CardListSemanticAction($1, $2);
}

card_definition: CARD IDENTIFIER OPEN_BRACE CLOSE_BRACE {
    $$ = CardSemanticAction($2);
}

/*
 * play_rule_section
 *   play_rule { <play_rule_list> }
 *   (absent) → NULL
 */
play_rule_section: PLAY_RULE OPEN_BRACE play_rule_list CLOSE_BRACE {
    $$ = $3;
}
| /* empty */ {
    $$ = NULL;
}

/*
 * play_rule_list — one or more play_rule_item entries
 */
play_rule_list: play_rule_item {
    $$ = PlayRuleListSemanticAction($1, NULL);
}
| play_rule_item play_rule_list {
    $$ = PlayRuleListSemanticAction($1, $2);
}

/*
 * play_rule_item — the six rule forms supported by the lexer:
 *
 *   allow if same_color
 *   allow if same_value
 *   allow if wild
 *   allow if any_card
 *   allow      <CardName> if played <CardName>
 *   cannot_play <CardName> if played <CardName>
 */
play_rule_item: ALLOW IF SAME_COLOR {
    $$ = PlayRuleSemanticAction(PLAY_RULE_ALLOW, NULL, PLAY_CONDITION_SAME_COLOR, NULL);
}
| ALLOW IF SAME_VALUE {
    $$ = PlayRuleSemanticAction(PLAY_RULE_ALLOW, NULL, PLAY_CONDITION_SAME_VALUE, NULL);
}
| ALLOW IF WILD {
    $$ = PlayRuleSemanticAction(PLAY_RULE_ALLOW, NULL, PLAY_CONDITION_WILD, NULL);
}
| ALLOW IF ANY_CARD {
    $$ = PlayRuleSemanticAction(PLAY_RULE_ALLOW, NULL, PLAY_CONDITION_ANY_CARD, NULL);
}
| ALLOW IDENTIFIER IF PLAYED IDENTIFIER {
    $$ = PlayRuleSemanticAction(PLAY_RULE_ALLOW, $2, PLAY_CONDITION_PLAYED_CARD, $5);
}
| CANNOT_PLAY IDENTIFIER IF PLAYED IDENTIFIER {
    $$ = PlayRuleSemanticAction(PLAY_RULE_CANNOT_PLAY, $2, PLAY_CONDITION_PLAYED_CARD, $5);
}

/*
 * turn_section
 *   turn { actions { <action_list> } }
 *   (absent) → NULL
 */
turn_section: TURN OPEN_BRACE ACTIONS OPEN_BRACE action_list CLOSE_BRACE CLOSE_BRACE {
    $$ = TurnSemanticAction($5);
}
| /* empty */ {
    $$ = NULL;
}

/*
 * action_list — one or more action_item entries (left-recursive to avoid stack growth)
 */
action_list: action_item {
    $$ = TurnActionListSemanticAction($1, NULL);
}
| action_item action_list {
    $$ = TurnActionListSemanticAction($1, $2);
}

/*
 * action_item
 *   may  IDENTIFIER   → ACTION_MAY
 *   must IDENTIFIER   → ACTION_MUST
 */
action_item: MAY IDENTIFIER {
    $$ = TurnActionSemanticAction(ACTION_MAY, $2);
}
| MUST IDENTIFIER {
    $$ = TurnActionSemanticAction(ACTION_MUST, $2);
}

/*
 * win_section
 *   win if empty_hand         → WinCondition { WIN_EMPTY_HAND, 0 }
 *   win if reach_points <N>   → WinCondition { WIN_REACH_POINTS, N }
 *   (absent)                  → NULL  (game has no explicit win condition)
 */
win_section: WIN IF EMPTY_HAND {
    $$ = WinEmptyHandSemanticAction();
}
| WIN IF REACH_POINTS INTEGER {
    $$ = WinPointsSemanticAction($4);
}
| /* empty */ {
    $$ = NULL;
}

%%
