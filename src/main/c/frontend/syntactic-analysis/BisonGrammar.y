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
    ColorList * color_list;
    CardAttribute * card_attribute;
    CardAttributeList * card_attribute_list;
    ActionNameList * action_name_list;
    RuleStatement * rule_statement;
    RuleStatementList * rule_statement_list;
    GameRule * game_rule;
    GameRuleList * game_rule_list;
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
%destructor { destroyColorList($$); } <color_list>
%destructor { destroyCardAttribute($$); } <card_attribute>
%destructor { destroyCardAttributeList($$); } <card_attribute_list>
%destructor { destroyActionNameList($$); } <action_name_list>
%destructor { destroyRuleStatement($$); } <rule_statement>
%destructor { destroyRuleStatementList($$); } <rule_statement_list>
%destructor { destroyGameRule($$); } <game_rule>
%destructor { destroyGameRuleList($$); } <game_rule_list>
%destructor { free($$); } <string>

/** Terminals */
%token <integer> INTEGER
%token <string> IDENTIFIER
%token <token> GAME PLAYERS DECK CARD HAND PLAY_RULE RULES TURN ACTIONS WIN
%token <token> IF ALLOW PLAYED CANNOT_PLAY ELSE MAY MUST
%token <token> EMPTY_HAND REACH_POINTS SAME_COLOR SAME_VALUE WILD ANY_CARD
%token <token> COLOR VALUE POINTS EFFECT
%token <token> RANGE OPEN_BRACE CLOSE_BRACE OPEN_COMMENT CLOSE_COMMENT COMMA IGNORED UNKNOWN

/** Non-terminals */
%type <program> program
%type <game> card_game
%type <player_range> players_section
%type <integer> hand_section
%type <card_list> deck_section card_list
%type <card> card_definition
%type <card_attribute_list> card_attribute_list
%type <card_attribute> card_attribute
%type <color_list> color_list
%type <action_name_list> actions_section action_name_list
%type <game_rule_list> rules_section game_rule_list
%type <game_rule> game_rule
%type <rule_statement_list> rule_statement_list
%type <rule_statement> rule_statement
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

card_game: GAME IDENTIFIER OPEN_BRACE players_section deck_section hand_section play_rule_section rules_section turn_section actions_section win_section CLOSE_BRACE {
    /* $2=name $4=players $5=deck $6=handSize $7=playRules $8=rules $9=turn $10=actions $11=win */
    $$ = GameSemanticAction($2, $4, $6, $5, $7, $8, $9, $10, $11);
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

card_definition: CARD IDENTIFIER OPEN_BRACE card_attribute_list CLOSE_BRACE {
    $$ = CardSemanticAction($2, $4);
}

/*
 * card_attribute_list — zero or more attributes inside a card body
 *   (absent) → NULL, which preserves backward compatibility with card Name { }
 */
card_attribute_list: card_attribute card_attribute_list {
    $$ = CardAttributeListSemanticAction($1, $2);
}
| /* empty */ {
    $$ = NULL;
}

/*
 * card_attribute — one of four attribute kinds:
 *
 *   color { red, blue, green, yellow }   → CARD_ATTR_COLOR  (list)
 *   color gold                           → CARD_ATTR_COLOR  (single)
 *   value 0..9                           → CARD_ATTR_VALUE  (range)
 *   value 5                              → CARD_ATTR_VALUE  (single)
 *   points 1..3                          → CARD_ATTR_POINTS (range)
 *   points 5                             → CARD_ATTR_POINTS (single)
 *   effect skip_next_player              → CARD_ATTR_EFFECT (no count)
 *   effect draw 2                        → CARD_ATTR_EFFECT (with count)
 *
 * SR note: "EFFECT IDENTIFIER" vs "EFFECT IDENTIFIER INTEGER" and
 *          "VALUE/POINTS INTEGER" vs "VALUE/POINTS INTEGER RANGE INTEGER"
 * are resolved by Bison's default shift preference — always correct here.
 */
card_attribute: COLOR OPEN_BRACE color_list CLOSE_BRACE {
    $$ = CardColorListSemanticAction($3);
}
| COLOR IDENTIFIER {
    $$ = CardColorSingleSemanticAction($2);
}
| VALUE INTEGER RANGE INTEGER {
    $$ = CardValueSemanticAction($2, $4);
}
| VALUE INTEGER {
    $$ = CardValueSemanticAction($2, $2);
}
| POINTS INTEGER RANGE INTEGER {
    $$ = CardPointsSemanticAction($2, $4);
}
| POINTS INTEGER {
    $$ = CardPointsSemanticAction($2, $2);
}
| EFFECT IDENTIFIER INTEGER {
    $$ = CardEffectSemanticAction($2, $3);
}
| EFFECT IDENTIFIER {
    $$ = CardEffectSemanticAction($2, 0);
}

/*
 * color_list — comma-separated list of color identifiers
 *   red, blue, green, yellow
 */
color_list: IDENTIFIER {
    $$ = ColorListSemanticAction($1, NULL);
}
| IDENTIFIER COMMA color_list {
    $$ = ColorListSemanticAction($1, $3);
}

/*
 * actions_section — declares the valid action names for the game (P2)
 *   actions { draw play choose_color }
 *   (absent) → NULL
 */
actions_section: ACTIONS OPEN_BRACE action_name_list CLOSE_BRACE {
    $$ = $3;
}
| /* empty */ {
    $$ = NULL;
}

/*
 * action_name_list — space-separated list of action identifiers
 */
action_name_list: IDENTIFIER {
    $$ = ActionNameListSemanticAction($1, NULL);
}
| IDENTIFIER action_name_list {
    $$ = ActionNameListSemanticAction($1, $2);
}

/*
 * rules_section — conditional game rules triggered by playing a card (P3)
 *   rules { if played CardName { statement+ } … }
 *   (absent) → NULL
 */
rules_section: RULES OPEN_BRACE game_rule_list CLOSE_BRACE {
    $$ = $3;
}
| /* empty */ {
    $$ = NULL;
}

/*
 * game_rule_list — one or more game_rule entries
 */
game_rule_list: game_rule {
    $$ = GameRuleListSemanticAction($1, NULL);
}
| game_rule game_rule_list {
    $$ = GameRuleListSemanticAction($1, $2);
}

/*
 * game_rule — a single conditional rule
 *   if played <CardName> { <rule_statement_list> }
 */
game_rule: IF PLAYED IDENTIFIER OPEN_BRACE rule_statement_list CLOSE_BRACE {
    /* $1=IF $2=PLAYED $3=IDENTIFIER(card) $4={ $5=list $6=} */
    $$ = GameRuleSemanticAction($3, $5);
}

/*
 * rule_statement_list — one or more rule statements
 * SR note: after a rule_statement, if lookahead is IDENTIFIER Bison shifts
 * (starts next statement). If CLOSE_BRACE, reduces. Correct by default.
 */
rule_statement_list: rule_statement {
    $$ = RuleStatementListSemanticAction($1, NULL);
}
| rule_statement rule_statement_list {
    $$ = RuleStatementListSemanticAction($1, $2);
}

/*
 * rule_statement — two forms:
 *   IDENTIFIER           action with no count  (skip_next_player, choose_color)
 *   IDENTIFIER INTEGER   action with count     (draw 2)
 *
 * Note: "next_player draw 2" is stored as two statements:
 *   RuleStatement("next_player",0) + RuleStatement("draw",2)
 */
rule_statement: IDENTIFIER {
    $$ = RuleStatementSemanticAction($1, 0);
}
| IDENTIFIER INTEGER {
    $$ = RuleStatementSemanticAction($1, $2);
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
    $$ = TurnActionSemanticAction(TURN_ACTION_PLAIN, $1, $2);
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
