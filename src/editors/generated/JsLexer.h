#ifndef JSHEADER_H
#define JSHEADER_H 1
#define JSIN_HEADER 1

#line 5 "/home/alex/Work/OpenSource/luminia-qt5/src/editors/generated/JsLexer.h"

#line 7 "/home/alex/Work/OpenSource/luminia-qt5/src/editors/generated/JsLexer.h"

#define  YY_INT_ALIGNED short int

/* A lexical scanner generated by flex */

#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 6
#define YY_FLEX_SUBMINOR_VERSION 4
#if YY_FLEX_SUBMINOR_VERSION > 0
#define FLEX_BETA
#endif

    /* The c++ scanner is a mess. The FlexLexer.h header file relies on the
     * following macro. This is required in order to pass the c++-multiple-scanners
     * test in the regression suite. We get reports that it breaks inheritance.
     * We will address this in a future release of flex, or omit the C++ scanner
     * altogether.
     */
    #define yyFlexLexer JSFlexLexer

#ifdef yyalloc
#define JSalloc_ALREADY_DEFINED
#else
#define yyalloc JSalloc
#endif

#ifdef yyrealloc
#define JSrealloc_ALREADY_DEFINED
#else
#define yyrealloc JSrealloc
#endif

#ifdef yyfree
#define JSfree_ALREADY_DEFINED
#else
#define yyfree JSfree
#endif

/* First, we deal with  platform-specific or compiler-specific issues. */

/* begin standard C headers. */

/* end standard C headers. */

/* flex integer type definitions */

#ifndef FLEXINT_H
#define FLEXINT_H

/* C99 systems have <inttypes.h>. Non-C99 systems may or may not. */

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

/* C99 says to define __STDC_LIMIT_MACROS before including stdint.h,
 * if you want the limit (max/min) macros for int types. 
 */
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif

#include <inttypes.h>
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
#else
typedef signed char flex_int8_t;
typedef short int flex_int16_t;
typedef int flex_int32_t;
typedef unsigned char flex_uint8_t; 
typedef unsigned short int flex_uint16_t;
typedef unsigned int flex_uint32_t;

/* Limits of integral types. */
#ifndef INT8_MIN
#define INT8_MIN               (-128)
#endif
#ifndef INT16_MIN
#define INT16_MIN              (-32767-1)
#endif
#ifndef INT32_MIN
#define INT32_MIN              (-2147483647-1)
#endif
#ifndef INT8_MAX
#define INT8_MAX               (127)
#endif
#ifndef INT16_MAX
#define INT16_MAX              (32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX              (2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX              (255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX             (65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#endif

#ifndef SIZE_MAX
#define SIZE_MAX               (~(size_t)0)
#endif

#endif /* ! C99 */

#endif /* ! FLEXINT_H */

/* begin standard C++ headers. */
#include <iostream>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
/* end standard C++ headers. */

/* TODO: this is always defined, so inline it */
#define yyconst const

#if defined(__GNUC__) && __GNUC__ >= 3
#define yynoreturn __attribute__((__noreturn__))
#else
#define yynoreturn
#endif

/* Size of default input buffer. */
#ifndef YY_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k.
 * Moreover, YY_BUF_SIZE is 2*YY_READ_BUF_SIZE in the general case.
 * Ditto for the __ia64__ case accordingly.
 */
#define YY_BUF_SIZE 32768
#else
#define YY_BUF_SIZE 16384
#endif /* __ia64__ */
#endif

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif

extern int yyleng;

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
	{

	std::streambuf* yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	int yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;

    int yy_bs_lineno; /**< The line count. */
    int yy_bs_column; /**< The column count. */

	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;

	};
#endif /* !YY_STRUCT_YY_BUFFER_STATE */

void *yyalloc ( yy_size_t  );
void *yyrealloc ( void *, yy_size_t  );
void yyfree ( void *  );

/* Begin user sect3 */

#define yytext_ptr yytext
#define YY_INTERACTIVE

#include <FlexLexer.h>

#ifdef YY_HEADER_EXPORT_START_CONDITIONS
#define INITIAL 0

#endif

#ifndef YY_NO_UNISTD_H
/* Special case for "unistd.h", since it is non-ANSI. We include it way
 * down here because we want the user's section 1 to have been scanned first.
 * The user has a chance to override it with an option.
 */
#include <unistd.h>
#endif

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE void *
#endif

#ifndef yytext_ptr
static void yy_flex_strncpy ( char *, const char *, int );
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen ( const char * );
#endif

#ifndef YY_NO_INPUT

#endif

/* Amount of stuff to slurp up with each read. */
#ifndef YY_READ_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k */
#define YY_READ_BUF_SIZE 16384
#else
#define YY_READ_BUF_SIZE 8192
#endif /* __ia64__ */
#endif

/* Number of entries by which start-condition stack grows. */
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif

/* Default declaration of generated scanner - a define so the user can
 * easily add parameters.
 */
#ifndef YY_DECL
#define YY_DECL_IS_OURS 1
#define YY_DECL int yyFlexLexer::yylex()
#endif /* !YY_DECL */

/* yy_get_previous_state - get the state just before the EOB char was reached */

#undef YY_NEW_FILE
#undef YY_FLUSH_BUFFER
#undef yy_set_bol
#undef yy_new_buffer
#undef yy_set_interactive
#undef YY_DO_BEFORE_ACTION

#ifdef YY_DECL_IS_OURS
#undef YY_DECL_IS_OURS
#undef YY_DECL
#endif

#ifndef JS_create_buffer_ALREADY_DEFINED
#undef yy_create_buffer
#endif
#ifndef JS_delete_buffer_ALREADY_DEFINED
#undef yy_delete_buffer
#endif
#ifndef JS_scan_buffer_ALREADY_DEFINED
#undef yy_scan_buffer
#endif
#ifndef JS_scan_string_ALREADY_DEFINED
#undef yy_scan_string
#endif
#ifndef JS_scan_bytes_ALREADY_DEFINED
#undef yy_scan_bytes
#endif
#ifndef JS_init_buffer_ALREADY_DEFINED
#undef yy_init_buffer
#endif
#ifndef JS_flush_buffer_ALREADY_DEFINED
#undef yy_flush_buffer
#endif
#ifndef JS_load_buffer_state_ALREADY_DEFINED
#undef yy_load_buffer_state
#endif
#ifndef JS_switch_to_buffer_ALREADY_DEFINED
#undef yy_switch_to_buffer
#endif
#ifndef JSpush_buffer_state_ALREADY_DEFINED
#undef yypush_buffer_state
#endif
#ifndef JSpop_buffer_state_ALREADY_DEFINED
#undef yypop_buffer_state
#endif
#ifndef JSensure_buffer_stack_ALREADY_DEFINED
#undef yyensure_buffer_stack
#endif
#ifndef JSlex_ALREADY_DEFINED
#undef yylex
#endif
#ifndef JSrestart_ALREADY_DEFINED
#undef yyrestart
#endif
#ifndef JSlex_init_ALREADY_DEFINED
#undef yylex_init
#endif
#ifndef JSlex_init_extra_ALREADY_DEFINED
#undef yylex_init_extra
#endif
#ifndef JSlex_destroy_ALREADY_DEFINED
#undef yylex_destroy
#endif
#ifndef JSget_debug_ALREADY_DEFINED
#undef yyget_debug
#endif
#ifndef JSset_debug_ALREADY_DEFINED
#undef yyset_debug
#endif
#ifndef JSget_extra_ALREADY_DEFINED
#undef yyget_extra
#endif
#ifndef JSset_extra_ALREADY_DEFINED
#undef yyset_extra
#endif
#ifndef JSget_in_ALREADY_DEFINED
#undef yyget_in
#endif
#ifndef JSset_in_ALREADY_DEFINED
#undef yyset_in
#endif
#ifndef JSget_out_ALREADY_DEFINED
#undef yyget_out
#endif
#ifndef JSset_out_ALREADY_DEFINED
#undef yyset_out
#endif
#ifndef JSget_leng_ALREADY_DEFINED
#undef yyget_leng
#endif
#ifndef JSget_text_ALREADY_DEFINED
#undef yyget_text
#endif
#ifndef JSget_lineno_ALREADY_DEFINED
#undef yyget_lineno
#endif
#ifndef JSset_lineno_ALREADY_DEFINED
#undef yyset_lineno
#endif
#ifndef JSget_column_ALREADY_DEFINED
#undef yyget_column
#endif
#ifndef JSset_column_ALREADY_DEFINED
#undef yyset_column
#endif
#ifndef JSwrap_ALREADY_DEFINED
#undef yywrap
#endif
#ifndef JSget_lval_ALREADY_DEFINED
#undef yyget_lval
#endif
#ifndef JSset_lval_ALREADY_DEFINED
#undef yyset_lval
#endif
#ifndef JSget_lloc_ALREADY_DEFINED
#undef yyget_lloc
#endif
#ifndef JSset_lloc_ALREADY_DEFINED
#undef yyset_lloc
#endif
#ifndef JSalloc_ALREADY_DEFINED
#undef yyalloc
#endif
#ifndef JSrealloc_ALREADY_DEFINED
#undef yyrealloc
#endif
#ifndef JSfree_ALREADY_DEFINED
#undef yyfree
#endif
#ifndef JStext_ALREADY_DEFINED
#undef yytext
#endif
#ifndef JSleng_ALREADY_DEFINED
#undef yyleng
#endif
#ifndef JSin_ALREADY_DEFINED
#undef yyin
#endif
#ifndef JSout_ALREADY_DEFINED
#undef yyout
#endif
#ifndef JS_flex_debug_ALREADY_DEFINED
#undef yy_flex_debug
#endif
#ifndef JSlineno_ALREADY_DEFINED
#undef yylineno
#endif
#ifndef JStables_fload_ALREADY_DEFINED
#undef yytables_fload
#endif
#ifndef JStables_destroy_ALREADY_DEFINED
#undef yytables_destroy
#endif
#ifndef JSTABLES_NAME_ALREADY_DEFINED
#undef yyTABLES_NAME
#endif

#line 121 "../luminia-qt5/src/editors/JsLexer.lex"


#line 440 "/home/alex/Work/OpenSource/luminia-qt5/src/editors/generated/JsLexer.h"
#undef JSIN_HEADER
#endif /* JSHEADER_H */
