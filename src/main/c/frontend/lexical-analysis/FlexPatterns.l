%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus KeywordLexemeAction(TokenLabel label);
extern CompilationStatus SymbolLexemeAction(TokenLabel label);
extern CompilationStatus IdentifierLexemeAction();
extern CompilationStatus UnknownLexemeAction();
%}

%x MULTILINE_COMMENT

whitespace      [[:space:]]+
identifier      [a-zA-Z_][a-zA-Z0-9_]*
integer         [[:digit:]]+

%%

"/*"                    { return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/" { return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+ { return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.    { return IgnoredLexemeAction(); }

"game"          { return KeywordLexemeAction(GAME); }
"players"       { return KeywordLexemeAction(PLAYERS); }
"deck"          { return KeywordLexemeAction(DECK); }
"card"          { return KeywordLexemeAction(CARD); }
"hand"          { return KeywordLexemeAction(HAND); }
"play_rule"     { return KeywordLexemeAction(PLAY_RULE); }
"rules"         { return KeywordLexemeAction(RULES); }
"turn"          { return KeywordLexemeAction(TURN); }
"actions"       { return KeywordLexemeAction(ACTIONS); }
"win"           { return KeywordLexemeAction(WIN); }

"if"            { return KeywordLexemeAction(IF); }
"allow"         { return KeywordLexemeAction(ALLOW); }
"played"        { return KeywordLexemeAction(PLAYED); }
"cannot_play"   { return KeywordLexemeAction(CANNOT_PLAY); }
"else"          { return KeywordLexemeAction(ELSE); }
"may"           { return KeywordLexemeAction(MAY); }
"must"          { return KeywordLexemeAction(MUST); }

"empty_hand"    { return KeywordLexemeAction(EMPTY_HAND); }
"reach_points"  { return KeywordLexemeAction(REACH_POINTS); }
"same_color"    { return KeywordLexemeAction(SAME_COLOR); }
"same_value"    { return KeywordLexemeAction(SAME_VALUE); }
"wild"          { return KeywordLexemeAction(WILD); }
"any_card"      { return KeywordLexemeAction(ANY_CARD); }

".."            { return SymbolLexemeAction(RANGE); }
"{"             { return SymbolLexemeAction(OPEN_BRACE); }
"}"             { return SymbolLexemeAction(CLOSE_BRACE); }
","             { return SymbolLexemeAction(COMMA); }

{integer}       { return IntegerLexemeAction(); }
{identifier}    { return IdentifierLexemeAction(); }

{whitespace}    { return IgnoredLexemeAction(); }
.               { return UnknownLexemeAction(); }

<<EOF>>         { return EOFLexemeAction(); }

%%

#include "FlexActions.h"
#include "FlexExport.h"

