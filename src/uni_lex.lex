%option noyywrap
%option yylineno
%option nounput
%option noinput
%top{
#include "unitheme.h"
#include "utils.h"
#include "parser.tab.h"
#include <string.h>

extern void yyerror(char *p);

char linebuf[512];
int yycolumn = 0;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
    yycolumn += yyleng;
}
/* --------------------------------------- */
/* STRING  https://regex101.com/r/9Navnv/1 */
/* REGEXPR https://regex101.com/r/GOrFiv/1 */
/* COMMENT https://regex101.com/r/4FTMVM/1 */
/* --------------------------------------- */

WHITESPACE [ \t]
BLANK      \s*\n*\s*
STRING     (\"((\\\")|(\\\\)|[^\\\"])+\"|\"\")
REGEXPR    (\/((\\\/)|(\\\\)|[^\/\\])+\/|\/\/)
COMMENT    [ \t]*#.*

IDENTIFIER [a-zA-Z0-9_-]+

LIST_REGEX "def"{WHITESPACE}+(\S+){BLANK}*"{"{REGEX}

%%
\n.*  { strncpy(linebuf, yytext+1, sizeof(linebuf)); yycolumn = -1; yyless(1); }
[ \t\v\f]											;
{COMMENT}                     /* skip comments */

{STRING}                      { yylval.str = strdup(yytext); return TSTR;  }
{REGEXPR}                     { yylval.str = strdup(yytext); return TRGXP; }
"string"                      { return KSTRING; }
"regex"                       { return KREGEX;  }

";"                           { return KSEMI;   }
","														{ return KCOMMA;  }
"."                           { return KPERIOD; }
"="														{ return KEQUALS; }

"{" 													{ return LBRACE; }
"}"														{ return RBRACE; }

"("                           { return LPAREN; }
")"                           { return RPAREN; }

"begindef"                    { return KBEGINDEF; }
"enddef"    							    { return KENDDEF;   }
"def"													{ return KDEF;      }

{IDENTIFIER}									{ yylval.str = strdup(yytext); return TIDENTIFIER; }
.															{ yyerror("lexical error (unexpected character)"); }
