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
%type <turn> turn_section turn_body
%type <turn_action> turn_statement
%type <turn_action_list> turn_statement_list
%type <play_rule_list> play_rule_section play_rule_list
%type <play_rule> play_rule_item

%%

program: card_game {
    $$ = GameProgramSemanticAction($1);
}

card_game: GAME IDENTIFIER OPEN_BRACE players_section deck_section hand_section play_rule_section turn_section win_section CLOSE_BRACE {
    /* $2=name  $4=players  $5=deck  $6=handSize  $7=playRules  $8=turn  $9=win */
    $$ = GameSemanticAction($2, $4, $6, $5, $7, $8, $9);
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
 *   turn { turn_body }
 *   (absent) → NULL
 */
turn_section: TURN OPEN_BRACE turn_body CLOSE_BRACE {
    $$ = $3;
}
| /* empty */ {
    $$ = NULL;
}

/*
 * turn_body — two forms from the spec:
 *
 *   Simple:      must play 1 / may draw 1 / …
 *   Conditional: if cannot_play { … } else { … }
 */
turn_body: turn_statement_list {
    $$ = TurnSimpleSemanticAction($1);
}
| IF CANNOT_PLAY OPEN_BRACE turn_statement_list CLOSE_BRACE ELSE OPEN_BRACE turn_statement_list CLOSE_BRACE {
    $$ = TurnConditionalSemanticAction($4, $8);
}

/*
 * turn_statement_list — one or more turn_statement entries
 */
turn_statement_list: turn_statement {
    $$ = TurnActionListSemanticAction($1, NULL);
}
| turn_statement turn_statement_list {
    $$ = TurnActionListSemanticAction($1, $2);
}

/*
 * turn_statement — six forms:
 *
 *   may  IDENTIFIER            ACTION_MAY,   count=0
 *   may  IDENTIFIER INTEGER    ACTION_MAY,   count=N
 *   must IDENTIFIER            ACTION_MUST,  count=0
 *   must IDENTIFIER INTEGER    ACTION_MUST,  count=N
 *   IDENTIFIER                 ACTION_PLAIN, count=0  (inside if/else blocks)
 *   IDENTIFIER INTEGER         ACTION_PLAIN, count=N  (inside if/else blocks)
 *
 * Bison shifts on INTEGER when it is the next lookahead, so the optional-integer
 * forms are unambiguous (default shift preference resolves any SR conflict).
 */
turn_statement: MAY IDENTIFIER {
    $$ = TurnActionSemanticAction(TURN_ACTION_MAY, $2, 0);
}
| MAY IDENTIFIER INTEGER {
    $$ = TurnActionSemanticAction(TURN_ACTION_MAY, $2, $3);
}
| MUST IDENTIFIER {
    $$ = TurnActionSemanticAction(TURN_ACTION_MUST, $2, 0);
}
| MUST IDENTIFIER INTEGER {
    $$ = TurnActionSemanticAction(TURN_ACTION_MUST, $2, $3);
}
| IDENTIFIER {
    $$ = TurnActionSemanticAction(TURN_ACTION_PLAIN, $1, 0);
}
| IDENTIFIER INTEGER {
    $$ = TurnActionSemanticAction(TURN_ACTION_PLAIN, $1, $3);
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
