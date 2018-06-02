/*! @file GlslLexer.lex
 * @brief Flex definitions for generating the GLSL lexer
 * @author Phil Rouse
 * @version 1.0
 * @date 07/05/2016
 */



%option c++
%option prefix="GLSL"

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

"attribute"               |
"const"                   |
"uniform"                 |
"varying"                 |
"layout"                  |
"centroid"                |
"flat"                    |
"smooth"                  |
"noperspective"           |
"break"                   |
"continue"                |
"do"                      |
"for"                     |
"while"                   |
"switch"                  |
"case"                    |
"default"                 |
"if"                      |
"else"                    |
"in"                      |
"out"                     |
"inout"                   |
"return"                  |
"invariant"               |
["low""medium""high"]"p"  |
"discard"                 |
"precision"               |
"struct"                  { return LexerScheme::StyleType::KEYWORD; }

(?#/* "/.?" added to override function rule*/)
[iu]?sampler(([123]D)|Cube)|([12]DArray)|2DRect|Buffer/{NID} { return LexerScheme::StyleType::DATATYPE; }
sampler(([123]D)|Cube)|([12]DArray)|((2DRect)Shadow)/{NID}   { return LexerScheme::StyleType::DATATYPE; }
[ibu]?vec[234]/{NID}                                         { return LexerScheme::StyleType::DATATYPE; }
mat[234](x[234])?/{NID}                                      { return LexerScheme::StyleType::DATATYPE; }
(u?int)/{NID}                                                { return LexerScheme::StyleType::DATATYPE; }
"float"|"bool"|"void"                                        { return LexerScheme::StyleType::DATATYPE; }

"//".*                        { return LexerScheme::StyleType::COMMENT; }
"/*"			                    { return LexerScheme::StyleType::MLC_START;}
"*/"			                    { return LexerScheme::StyleType::MLC_END;}
^#.*                          { return LexerScheme::StyleType::HASHCOMMENT; }
{SPACE}                       { return LexerScheme::StyleType::WHITESPACE; }

[\*\+\-\/\>\<\%\^\|\&\!\=]?\=? |
&&   |
\|\| |
\^\^ |
\.\= |
\?:  |
">>"|"<<"\=?                  { return LexerScheme::StyleType::OPERATOR; }

{ID}\.{NWORD}+                { return LexerScheme::StyleType::ILLEGAL; }

gl_{ALNUM}*                   { return LexerScheme::StyleType::GLOBAL; } 

"true"|"false"                { return LexerScheme::StyleType::NUMBER; }

[-+]?{DIGIT}+[lu]?            { return LexerScheme::StyleType::NUMBER; }
[-+]?{DIGIT}+\.?{DIGIT}*f?    { return LexerScheme::StyleType::NUMBER; }

[\.]{2,}                      { return LexerScheme::StyleType::ILLEGAL; }

{ID}{NID}*/\(                 { return LexerScheme::StyleType::FUNCTION; }

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
