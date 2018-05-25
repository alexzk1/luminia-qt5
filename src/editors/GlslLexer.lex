/*! @file GlslLexer.lex
 * @brief Flex definitions for generating the GLSL lexer
 * @author Phil Rouse
 * @version 1.0
 * @date 07/05/2016
 */



%option c++

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
[iu]?sampler(([123]D)|Cube)|([12]DArray)|2DRect|Buffer/.? { return LexerScheme::StyleType::DATATYPE; }
sampler(([123]D)|Cube)|([12]DArray)|((2DRect)Shadow)/.?   { return LexerScheme::StyleType::DATATYPE; }
[ibu]?vec[234]/.?                                         { return LexerScheme::StyleType::DATATYPE; }
mat[234](x[234])?/.?                                      { return LexerScheme::StyleType::DATATYPE; }
(u?int)|"float"|"bool"|"void"/.?                          { return LexerScheme::StyleType::DATATYPE; }

"//".*                        { return LexerScheme::StyleType::COMMENT; }
^#.*                          { return LexerScheme::StyleType::HASHCOMMENT; }
[ \t\n\r]                     { return LexerScheme::StyleType::WHITESPACE; }

[\*\+\-\/\>\<\%\^\|\&\!\=]?\=? |
&&   |
\|\| |
\^\^ |
\.\= |
\?:  |
">>"|"<<"\=?                  { return LexerScheme::StyleType::OPERATOR; }

gl_[a-zA-Z0-9]+               { return LexerScheme::StyleType::GLOBAL; } 

[Tt]rue |
[Ff]alse |
-?+?[0-9]+"."?[0-9]*f?l?u?    { return LexerScheme::StyleType::NUMBER; }

[a-zA-Z0-9]+/\(               { return LexerScheme::StyleType::FUNCTION; }
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
