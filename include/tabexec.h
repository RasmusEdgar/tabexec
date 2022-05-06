// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
// Copyright 2021 Rasmus Edgar
#include <exec/memory.h>
#include <dos/dosextens.h>
#include <dos/rdargs.h>
#include <dos/exall.h>
#include <dos/dostags.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <clib/utility_protos.h>
#include <stdbool.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/icon.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/gadtools_protos.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>

#include <intuition/sghooks.h>

/* Buffersize to receive filenames in */
#define BUFFERSIZE 8192

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (26)

// Converts key current character into index
// use only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

#define MAX(A, B)               ((A) > (B) ? (A) : (B))

#define MYSTRGADWIDTH (215)

#define SG_STRLEN     (214)

#define DONE 0
#define RUNNING 1

#define DEFCON_PRE "CON:0/0/550/350/"
#define DEFCON_POST "/AUTO/CLOSE/WAIT"

#define TT_MAX_LENGTH 128
#define FN_MAX_LENGTH 215

#define OPTTYPE_ID 1
#define PATHS_ID 100
#define FPEN_ID 101
#define BPEN_ID 102
#define FPEN_SEP_ID 103
#define BPEN_SEP_ID 104
#define PSTACK_ID 105

#define DEFAULT_PATH "SYS:C,SYS:Utilities,SYS:Tools"

#define ESCAPE_C (unsigned char)'\033'
#define PLUS_C (unsigned char)'\012' //ctrl-j
#define MINUS_C (unsigned char)'\014' //ctrl-l
#define TAB_C (unsigned char)'\t'
#define BACKSPACE_C (unsigned char)'\b'
#define SPACE_C (unsigned char)' '
#define REPLACE_C (unsigned char)(0x0007)
#define INSERT_C (unsigned char)(0x0008)

#define DEFAULT_BPEN 3U
#define DEFAULT_FPEN 1U

#define DEFAULT_STACK (unsigned long)16384

#define LOFFS 0
#define TOFFS 2

struct ExecBase *SysBase;
struct ExAllData *tead, *ead, *buffer;

struct Screen *screen;
struct Window *dawin;

typedef struct {
	unsigned char fpen[1];
	unsigned char bpen[1];
	unsigned char fpen_sep[1];
	unsigned char bpen_sep[1];
} Colors;
Colors colors;

struct Vars {
	struct Gadget       sgg_Gadget;
	struct StringInfo   sgg_StrInfo;
	struct StringExtend sgg_Extend;
	struct Hook         sgg_Hook;
	UBYTE               sgg_Buff[SG_STRLEN];
	UBYTE               sgg_WBuff[SG_STRLEN];
	UBYTE               sgg_UBuff[SG_STRLEN];
};

struct TextExtent teinit = {
	.te_Width = 0,
	.te_Height = 0,
	.te_Extent.MinY = 0,
	.te_Extent.MaxY = 0,
	.te_Extent.MinX = 0,
	.te_Extent.MaxX = 0
};


struct IntuiText mtext = {
	.TopEdge = 0,
	.LeftEdge = 0,
	.ITextFont = NULL,
	.DrawMode = JAM2, //-V2568
	.FrontPen = 1, //-V2568
	.BackPen = 3, //-V2568
	.IText = NULL,
	.NextText = NULL
};

struct IntuiText stext = {
	.TopEdge = 0,
	.LeftEdge = 0,
	.ITextFont = NULL,
	.DrawMode = JAM2, //-V2568
	.FrontPen = 1, //-V2568
	.BackPen = 3, //-V2568
	.IText = NULL,
	.NextText = NULL
};

typedef struct {
        char *optname;
        long id;
        int tt_type;
} Opts;

char TYPE_PATHS[] = "PATHS";
char TYPE_FPEN[] = "FPEN";
char TYPE_BPEN[] = "BPEN";
char TYPE_FPEN_SEP[] = "FPEN_SEP";
char TYPE_BPEN_SEP[] = "BPEN_SEP";
char TYPE_PSTACK[] = "PSTACK";

Opts defopts[] = {
        { TYPE_PATHS, PATHS_ID, OPTTYPE_ID },
        { TYPE_FPEN, FPEN_ID, OPTTYPE_ID },
        { TYPE_BPEN, BPEN_ID, OPTTYPE_ID },
        { TYPE_FPEN_SEP, FPEN_SEP_ID, OPTTYPE_ID },
        { TYPE_BPEN_SEP, BPEN_SEP_ID, OPTTYPE_ID },
        { TYPE_PSTACK, PSTACK_ID, OPTTYPE_ID },
};
