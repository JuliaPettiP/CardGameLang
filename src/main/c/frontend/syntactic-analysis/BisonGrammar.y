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

	/** Terminals (Dados que vêm do Lexer)[cite: 3] */
	signed int integer;
	char * string;
	TokenLabel token;

	/** Non-terminals (Nós da Árvore Sintática) */
	Program * program;
	Game * game;
	PlayerRange * player_range;
}

/** 
 * Destrutores. Estas funções são executadas se o Bison falhar a meio (ex: erro de sintaxe),
 * garantindo que a memória alocada até ao momento é libertada. Isto evita os erros de
 * status 1 causados pelas memory leaks e satisfaz o AddressSanitizer. 
 */
%destructor { destroyGame($$); } <game>
%destructor { destroyPlayerRange($$); } <player_range>
%destructor { free($$); } <string>

/** Terminals (Keywords do jogo definidas no Flex)[cite: 3] */
%token <integer> INTEGER
%token <string> IDENTIFIER

%token <token> GAME
%token <token> PLAYERS
%token <token> DECK
%token <token> CARD
%token <token> HAND
%token <token> PLAY_RULE
%token <token> RULES
%token <token> TURN
%token <token> ACTIONS
%token <token> WIN

%token <token> IF
%token <token> ALLOW
%token <token> PLAYED
%token <token> CANNOT_PLAY
%token <token> ELSE
%token <token> MAY
%token <token> MUST

%token <token> EMPTY_HAND
%token <token> REACH_POINTS
%token <token> SAME_COLOR
%token <token> SAME_VALUE
%token <token> WILD
%token <token> ANY_CARD

%token <token> RANGE
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_COMMENT
%token <token> CLOSE_COMMENT
%token <token> COMMA

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals (Tipos devolvidos pelas regras abaixo) */
%type <program> program
%type <game> card_game
%type <player_range> players_section
%type <integer> hand_section

%%

// Regra raiz do compilador
program: card_game { 
    $$ = GameProgramSemanticAction($1); 
}

// A estrutura principal do jogo
card_game: GAME IDENTIFIER OPEN_BRACE players_section hand_section win_section CLOSE_BRACE { 
    $$ = GameSemanticAction($2, $4, $5); 
}

// Secção de jogadores: suporta tanto "players 2" como "players 2 .. 4"
players_section: PLAYERS INTEGER RANGE INTEGER {
    $$ = PlayerRangeSemanticAction($2, $4);
}
| PLAYERS INTEGER {
    $$ = PlayerRangeSemanticAction($2, $2);
}

// Secção da mão devolve diretamente um inteiro (não precisa de AST extra para já)
hand_section: HAND INTEGER {
    $$ = $2;
}

// A secção de vitória provisória. Por agora, não tem ação semântica associada[cite: 3].
win_section: WIN IF EMPTY_HAND {
    // Fica vazio até expandirmos o AST com as regras de vitória.
}

%%