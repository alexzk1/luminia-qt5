/*! @file JsLexer.lex
 * @brief Flex definitions for generating the JavaScript lexer
 * @author Phil Rouse (original GLSL), JS by Oleksiy Zaharov
 * @version 1.0
 * @date 25/May/2018
 */



%option c++
%option prefix="JS"

DIGIT    [0-9]
WORD     [a-zA-Z_]
ALNUM    [0-9a-zA-Z_]
SPACE    [\t\n\r ]

ID	     {WORD}{ALNUM}*
NID      [^a-zA-Z_0-9]
NWORD    [^a-zA-Z_]

%{
  #include "../lexerscheme.h"
%}

%%

"abstract"      |
"break"         |
"case"          |
"catch"         |
"class"         |
"const"         |
"continue"      |
"debugger"      |
"default"       |
"delete"        |
"do"            |
"double"        |
"else"          |
"enum"          |
"export"        |
"extends"       |
"final"         |
"finally"       |
"for"           |
"function"      |
"goto"          |
"if"            |
"implements"    |
"import"        |
"in"            |
"interface"     |
"native"        |
"new"           |
"package"       |
"private"       |
"protected"     |
"public"        |
"return"        |
"static"        |
"super"         |
"switch"        |
"synchronized"  |
"this"          |
"throw"         |
"throws"        |
"transient"     |
"try"           |
"volatile"      |
"while"         |
"with"           { return LexerScheme::StyleType::KEYWORD; }

\"(\\.|[^"\\])*\"         {return LexerScheme::StyleType::STRING;}
'(\\.|[^'\\])*'           {return LexerScheme::StyleType::STRING;}

"var"|"boolean"|"int"|"byte"|"char"|"float"|"long"|"short"|"void"|"..."     { return LexerScheme::StyleType::DATATYPE; }

"//".*                        { return LexerScheme::StyleType::COMMENT; }
"/*"			                    { return LexerScheme::StyleType::MLC_START;}
"*/"			                    { return LexerScheme::StyleType::MLC_END;}

{SPACE}                       { return LexerScheme::StyleType::WHITESPACE; }

"typeof"        |
"instanceof"    |
[\*\+\-\/\>\<\%\^\|\&\!\=]?\=? |
&&   |
\|\| |
\.\= |
\?:  |
">>"|">>>"|"<<"\=?|"==="|"=="|"!==" { return LexerScheme::StyleType::OPERATOR; }

{ID}\.{NWORD}+                  { return LexerScheme::StyleType::ILLEGAL; }

"true"|"false"|"null"           { return LexerScheme::StyleType::NUMBER; }

[-+]?{DIGIT}+\.?{DIGIT}*        { return LexerScheme::StyleType::NUMBER; }

^{DIGIT}+\.?{DIGIT}*{WORD}+      { return LexerScheme::StyleType::ILLEGAL; }
{NID}{DIGIT}+\.?{DIGIT}*{WORD}+  { return LexerScheme::StyleType::ILLEGAL; }

".."                             { return LexerScheme::StyleType::ILLEGAL; }
\.{4,}                           { return LexerScheme::StyleType::ILLEGAL; }

{ID}{NID}*/\(                   { return LexerScheme::StyleType::FUNCTION; }

[\(\)\{\}\[\]] |
[#@\?:;,_\~\\] |
[a-zA-Z0-9]+   |
"."                           { return LexerScheme::StyleType::DEFAULT; }


�*                            {
                                yyleng = 1;
                                return LexerScheme::StyleType::ILLEGAL;
                              }
.                             { return LexerScheme::StyleType::ILLEGAL; }


%%

int yyFlexLexer::yywrap()   { return 1;}
