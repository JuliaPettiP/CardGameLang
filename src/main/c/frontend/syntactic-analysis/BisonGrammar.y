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
}

%destructor { destroyGame($$); } <game>
%destructor { destroyPlayerRange($$); } <player_range>
%destructor { destroyCard($$); } <card>
%destructor { destroyCardList($$); } <card_list>
%destructor { free($$); } <string>

/** Terminals (Mantém todos os que a Julia já fez) */
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
%type <integer> hand_section win_section
%type <card_list> deck_section card_list
%type <card> card_definition

%%

program: card_game { 
    $$ = GameProgramSemanticAction($1); 
}

card_game: GAME IDENTIFIER OPEN_BRACE players_section hand_section deck_section win_section CLOSE_BRACE { 
    $$ = GameSemanticAction($2, $4, $5, $6, $7); 
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

// O Baralho começa com DECK e devolve uma lista de cartas
deck_section: DECK OPEN_BRACE card_list CLOSE_BRACE {
    $$ = $3;
}

// Uma lista é formada por uma carta...
card_list: card_definition {
    $$ = CardListSemanticAction($1, NULL);
}
// ...ou por uma carta seguida de mais cartas
| card_definition card_list {
    $$ = CardListSemanticAction($1, $2);
}

// A definição individual de uma carta
card_definition: CARD IDENTIFIER OPEN_BRACE CLOSE_BRACE {
    $$ = CardSemanticAction($2);
}

win_section: WIN IF EMPTY_HAND {
    $$ = 1;
}
| /* VAZIO */ {
    $$ = 0;
}

%%