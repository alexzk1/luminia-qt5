/*! @file GlslLexer.lex
 * @brief Flex definitions for generating the GLSL lexer
 * @author Phil Rouse
 * @version 1.0
 * @date 07/05/2016
 */



%option c++
%option prefix="JS"

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

"var"|"boolean"|"int"|"byte"|"char"|"float"|"long"|"short"|"void"|"..."/.?     { return LexerScheme::StyleType::DATATYPE; }

"//".*                        { return LexerScheme::StyleType::COMMENT; }
"/*"			                    { return LexerScheme::StyleType::MLC_START;}
"*/"			                    { return LexerScheme::StyleType::MLC_END;}

[ \t\n\r]                     { return LexerScheme::StyleType::WHITESPACE; }

"typeof"        |
"instanceof"    |
[\*\+\-\/\>\<\%\^\|\&\!\=]?\=? |
&&   |
\|\| |
\.\= |
\?:  |
">>"|">>>"|"<<"\=?|"==="|"=="|"!==" { return LexerScheme::StyleType::OPERATOR; }

true |
false |
null |
-?+?[0-9]+"."?[0-9]*          { return LexerScheme::StyleType::NUMBER; }

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
