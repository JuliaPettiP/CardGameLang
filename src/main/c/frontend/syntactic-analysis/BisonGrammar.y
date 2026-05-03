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

	/** Terminals. */
	signed int integer;
	char * string;
	TokenLabel token;

	/** Non-terminals. */
	Program * program;
	Game * game;
}
/** Todavia no usamos destructor
* %destructor { destroyGame($$); } <game>
*/

/** Terminals. */
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

/** Non-terminals. */
%type <program> program
%type <game> card_game

%%

program:
	card_game
		{ $$ = GameProgramSemanticAction($1); }
;

card_game:
	GAME IDENTIFIER OPEN_BRACE game_body CLOSE_BRACE
		{ $$ = GameSemanticAction($2); }
;

game_body:
	players_section hand_section win_section
;

players_section:
	PLAYERS INTEGER
;

hand_section:
	HAND INTEGER
;

win_section:
	WIN IF EMPTY_HAND
;

%%


/* CODIGO ANTERIOR
%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
 /*
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	signed int integer;
	TokenLabel token;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
 /*
%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyFactor($$); } <factor>

/** Terminals. */
/*
%token <integer> INTEGER
%token <token> ADD
%token <token> CLOSE_BRACE
%token <token> CLOSE_COMMENT
%token <token> CLOSE_PARENTHESIS
%token <token> DIV
%token <token> MUL
%token <token> OPEN_BRACE
%token <token> OPEN_COMMENT
%token <token> OPEN_PARENTHESIS
%token <token> SUB

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
/*
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
 /*
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: expression											{ $$ = ExpressionProgramSemanticAction($1); }
	;

expression: expression[left] ADD expression[right]			{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor												{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS		{ $$ = ExpressionFactorSemanticAction($2); }
	| constant												{ $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER											{ $$ = IntegerConstantSemanticAction($1); }
	;

%%

*/