// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
// Copyright 2021 Rasmus Edgar
#include "../include/tabexec.h"

/*
The following functions:
appenditem
match
fstrncmp
fstrstr

have been copied and slightly modified from the dmenu project, which
can be found here: https://tools.suckless.org/dmenu/

License and attribution included below.

MIT/X Consortium License

© 2006-2019 Anselm R Garbe <anselm@garbe.ca>
© 2006-2008 Sander van Dijk <a.h.vandijk@gmail.com>
© 2006-2007 Michał Janeczek <janeczek@gmail.com>
© 2007 Kris Maglione <jg@suckless.org>
© 2009 Gottox <gottox@s01.de>
© 2009 Markus Schnalke <meillo@marmaro.de>
© 2009 Evan Gates <evan.gates@gmail.com>
© 2010-2012 Connor Lane Smith <cls@lubutu.com>
© 2014-2020 Hiltjo Posthuma <hiltjo@codemadness.org>
© 2015-2019 Quentin Rameau <quinq@fifth.space>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.*/

struct item {
	char *text;
	struct item *left, *right;
	int pathid;
};

static struct item *items = NULL;
static struct item *matches, *matchend;
static struct item *curr, *sel = NULL;
static char text[FN_MAX_LENGTH] = "";

int main(void);
static void freetext(void);
static int attachtooltypes(void);
static int getexecs(unsigned char *s, int pathid);
static int init_dawin(void);
static int (*fstrncmp)(const char *, const char *, size_t) = strncmp;
static char *(*fstrstr)(const char *, const char *) = strstr;
static void appenditem(struct item *item, struct item **list, struct item **last);
static int match(void);
static int match_to_win(unsigned char *strbuf);
static void lockbasescreen(unsigned long *il, struct Screen **s);
static void unlockbasescreen(unsigned long *il, struct Screen **s);
static unsigned long ilock;
static int handlekeys(void);
static int exec_match(unsigned char *em);
static short bufmov(unsigned char *wb, char *s);
static size_t strnlen(const char *s, size_t maxlen);
static void cleanup(void);
static struct Vars *vars;
static struct DrawInfo *drawinfo;
static unsigned long hook_routine(struct Hook *hook, struct SGWork *sgw, unsigned long *msg);
static void init_hook(struct Hook *hook, unsigned long (*ccode)(struct Hook *hook, struct SGWork *sgw, unsigned long *msg));
unsigned long __saveds hookEntry(struct Hook *hookptr __asm("a0"), void  *object __asm("a2"), void  *message __asm("a1"));
static short int textwidth(int lei, unsigned char * it);
static void setdefaults(void);
static int pup_paths(const char * s);
static bool filter(const char *s);
static int custom_exec_n = 0;
static unsigned char space[] = " ";
static unsigned char sep[] = " || ";
static unsigned short spacew;
static char deadsignum = -1;
static unsigned long deadsig, uportsig;
static struct Task *tabexectask = NULL;
static unsigned short winh = 0;
static char *paths[TT_MAX_LENGTH];
static char *freepaths, *ppath;
static int pathc = 0;
static int exei = 0;
static int tabc = 0;
static unsigned long pstack_size = 0;

int main(void)
{
	int state = DONE;

	setdefaults();

	state = attachtooltypes();
	if (state == DONE) {
		return EXIT_FAILURE;
	}

	if (paths[0] == NULL) {
		state = pup_paths(DEFAULT_PATH);
		if (state == DONE) {
			return EXIT_FAILURE;
		}
	}

	for (int i = 0; i < pathc; ++i) {
		state = getexecs((unsigned char *)paths[i], i);
		if (state == DONE) {
			return EXIT_FAILURE;
		}
	}

        if ((deadsignum = AllocSignal(-1)) == (char)-1) {
                return EXIT_FAILURE;
        }

	deadsig = 1UL << (unsigned long)deadsignum;
	tabexectask = FindTask(NULL);

	state = init_dawin();

	if (state) {
		uportsig = 1UL << dawin->UserPort->mp_SigBit;

		while(state) {
			unsigned long signals = Wait(uportsig | deadsig);
				if (signals & deadsig) {
					state = DONE;
				}

				if (signals & uportsig) {
					state = handlekeys();
				}
		}
	}

	cleanup();
	return state;
}

static void setdefaults(void)
{
	colors.fpen[0] = DEFAULT_FPEN;
	colors.bpen[0] = DEFAULT_BPEN;
	colors.fpen_sep[0] = DEFAULT_FPEN;
	colors.bpen_sep[0] = DEFAULT_FPEN;
	pstack_size = DEFAULT_STACK;
}

static int pup_paths(const char * s)
{
	if((freepaths = strdup(s)) == NULL) {
		return DONE;
	}
	paths[pathc] = ppath = strtok(freepaths, ",");
	for (pathc++; (ppath = strtok(NULL, ",")); pathc++) {
		paths[pathc] = ppath;
	}
	return RUNNING;
}

static int attachtooltypes(void)
{
	int state = RUNNING;
	size_t optarrsize;
	static struct DiskObject *diskobj;
	static struct Library *iconbase;
	static unsigned char iconlib[] = "icon.library";
	static unsigned char diskobjname[] = "tabexec:tabexec";

	optarrsize = sizeof(defopts) / sizeof(*defopts);

        if (!(iconbase = OpenLibrary(iconlib, 37))) {
                return DONE;
        }

        if ((diskobj = GetDiskObject(diskobjname)) == NULL) {
                return DONE;
        }

        for (size_t i = 0; i < optarrsize ; ++i) {
                char *tt_optvalue = (char *)FindToolType(diskobj->do_ToolTypes, (unsigned char *)defopts[i].optname);
                if (state == true && (tt_optvalue) && ((strnlen(tt_optvalue, TT_MAX_LENGTH) < (size_t)TT_MAX_LENGTH))) {
			switch (defopts[i].id) {
                                case PATHS_ID:
					state = pup_paths((const char *)tt_optvalue);
                                        break;
				case FPEN_ID:
					colors.fpen[0] = (unsigned char)strtoul((const char *)tt_optvalue, (char **)NULL, 10);
					break;
				case BPEN_ID:
					colors.bpen[0] = (unsigned char)strtoul((const char *)tt_optvalue, (char **)NULL, 10);
					break;
				case FPEN_SEP_ID:
					colors.fpen_sep[0] = (unsigned char)strtoul((const char *)tt_optvalue, (char **)NULL, 10);
					break;
				case BPEN_SEP_ID:
					colors.bpen_sep[0] = (unsigned char)strtoul((const char *)tt_optvalue, (char **)NULL, 10);
					break;
				case PSTACK_ID:
					pstack_size = (unsigned char)strtoul((const char *)tt_optvalue, (char **)NULL, 10);
					break;
                                default:
                                        // Do nothing
                                        break;
			}
		}
	}
	CloseLibrary(iconbase);
	FreeDiskObject(diskobj);
	return state;
}

static int getexecs(unsigned char *source, int pathid)
{
	struct DosLibrary *DOSBase;
	struct ExAllControl *excontrol = NULL;
	BPTR sourcelock;
	LONG error;
	int state = RUNNING;
	unsigned char dlib[] = "dos.library";
	static struct item *tmpitems = NULL;

	/* set up SysBase */
	SysBase = (struct ExecBase *)(*((struct Library **) 4)); //-V2545

	/* Fail silently if < 37 */
	if (!(DOSBase = (struct DosLibrary *) OpenLibrary(dlib, 37))) { //-V2545
		return DONE;
	}

	if (!(buffer = AllocMem(BUFFERSIZE, MEMF_CLEAR))) { //-V2544
		return DONE;
	}

	if ((sourcelock = Lock(source, SHARED_LOCK)) == 0) {
		UnLock(sourcelock);
		state = DONE;
	}

	if (state == RUNNING && (excontrol = AllocDosObject(DOS_EXALLCONTROL, NULL))) {
		excontrol->eac_LastKey = 0U;

		bool exmore = ExAll(sourcelock, buffer, BUFFERSIZE, ED_NAME, excontrol);
		error = IoErr();

		if ((!exmore && (error != ERROR_NO_MORE_ENTRIES))) {
			state = DONE;
		}

		if (excontrol->eac_Entries != 0U) {
			ead = buffer;
			if (ead == NULL) {
				return DONE;
			}

			char buf[sizeof text] = {'\0'};
			size_t size = 0;
			do {
				char *p;
				if (exei + 1 >= (int)(size / sizeof *items)) {
					if (!(tmpitems = realloc(items, (size += (unsigned int)BUFFERSIZE)))) {
						return DONE;
					} else {
						items = tmpitems;
					}
				}
				if ((p = strchr(buf, '\n'))) {
					 *p = '\0';
				}
				if (filter((const char *)ead->ed_Name)) {
					if (!(items[exei].text = strdup((const char *)(ead->ed_Name)))) {
						return DONE;
					}
					items[exei].pathid = pathid;
					exei++;
				}
				ead = ead->ed_Next;
			} while (ead);
		}
        }
	FreeMem(buffer, BUFFERSIZE);
	FreeDosObject(DOS_EXALLCONTROL, excontrol);
        CloseLibrary((struct Library *) DOSBase); //-V2545
	return state;
}

static bool filter(const char *s)
{
	struct filter_list {
		const char *filter;
	} filter_list[3];
	int filters = 2;
	int i;

	filter_list[0].filter = ".info";
	filter_list[1].filter = "?.";
	filter_list[2].filter = NULL;

	for (i = 0; filter_list[i].filter; i++) {
		if ((strstr(s, filter_list[i].filter) != NULL)) {
			break;
		}
	}

	if (i == filters) {
		return true;
	} else {
		return false;
	}
}

static int init_dawin(void)
{
        struct TagItem tagitem[8];
	unsigned long dawin_h = 20;

        lockbasescreen(&ilock, &screen);
	if ((drawinfo = GetScreenDrawInfo(screen)) == 0) {
                unlockbasescreen(&ilock, &screen);
		return DONE;
	}

	//vars = (struct Vars *)AllocMem(sizeof(struct Vars), MEMF_CLEAR); //-V2544
	vars = (struct Vars *)malloc(sizeof(struct Vars)); //-V2544
	if (vars != NULL) {
		vars->sgg_Extend.Pens[0] = (unsigned char)drawinfo->dri_Pens[FILLTEXTPEN];
		vars->sgg_Extend.Pens[1] = (unsigned char)drawinfo->dri_Pens[FILLPEN];
		vars->sgg_Extend.ActivePens[0] = (unsigned char)drawinfo->dri_Pens[FILLTEXTPEN];
		vars->sgg_Extend.ActivePens[1] = (unsigned char)drawinfo->dri_Pens[FILLPEN];
		vars->sgg_Extend.EditHook = &(vars->sgg_Hook);
		vars->sgg_Extend.WorkBuffer = vars->sgg_WBuff;

		vars->sgg_StrInfo.Buffer = vars->sgg_Buff;
		vars->sgg_StrInfo.UndoBuffer = vars->sgg_UBuff;
		vars->sgg_StrInfo.MaxChars = SG_STRLEN;
		vars->sgg_StrInfo.Extension = &(vars->sgg_Extend);

		vars->sgg_Gadget.LeftEdge = 4;
		vars->sgg_Gadget.TopEdge = 2;
		vars->sgg_Gadget.Width = MYSTRGADWIDTH;
		vars->sgg_Gadget.Height = (short)screen->RastPort.TxHeight;
		vars->sgg_Gadget.Flags = GFLG_GADGHCOMP | GFLG_STRINGEXTEND; //-V2544 //-V2568
		vars->sgg_Gadget.Activation = GACT_RELVERIFY; //-V2544 //-V2568
		vars->sgg_Gadget.GadgetType = GTYP_STRGADGET; //-V2544 //-V2568
		vars->sgg_Gadget.SpecialInfo = &(vars->sgg_StrInfo);
	} else {
                unlockbasescreen(&ilock, &screen);
		return DONE;
	}

	winh = (unsigned short)(vars->sgg_Gadget.Height + 5);
        unsigned long swidth = (unsigned long)screen->Width;

        tagitem[0].ti_Tag = WA_Width; //-V2544 //-V2568
        tagitem[0].ti_Data = swidth;
        tagitem[1].ti_Tag = WA_Height; //-V2544 //-V2568
        tagitem[1].ti_Data = dawin_h;
        tagitem[2].ti_Tag = WA_Top; //-V2544 //-V2568
        tagitem[2].ti_Data = 0UL;
        tagitem[3].ti_Tag = WA_Borderless; //-V2544 //-V2568
        tagitem[3].ti_Data = 1; //-V2568
        tagitem[4].ti_Tag = WA_SmartRefresh; //-V2544 //-V2568
        tagitem[4].ti_Data = 1; //-V2568
        tagitem[5].ti_Tag = WA_IDCMP; //-V2544 //-V2568
        tagitem[5].ti_Data = IDCMP_GADGETUP|STRINGIDCMP|IDCMP_ACTIVEWINDOW|IDCMP_REFRESHWINDOW|IDCMP_CHANGEWINDOW; //-V2544 //-V2568
	tagitem[6].ti_Tag = WA_Gadgets; //-V2544 //-V2568
	tagitem[6].ti_Data = (unsigned long)&(vars->sgg_Gadget);
        tagitem[7].ti_Tag = TAG_DONE; //-V2544 //-V2568

        dawin = OpenWindowTagList(NULL, tagitem);

        if (!dawin) {
                unlockbasescreen(&ilock, &screen);
                return DONE;
        }

	spacew = (unsigned short)textwidth(0, space);

	SetAPen(dawin->RPort, colors.fpen[0]);
	SetBPen(dawin->RPort, colors.bpen[0]);

	SetRast(dawin->RPort, colors.fpen[0]);
	stext.LeftEdge = (short)((unsigned short)MYSTRGADWIDTH + spacew);
	stext.IText = sep;
	PrintIText(dawin->RPort, &stext, LOFFS, TOFFS);
	stext.LeftEdge = textwidth(stext.LeftEdge, sep);

	ActivateWindow(dawin);
	(void)ActivateGadget(&(vars->sgg_Gadget), dawin, 0);

	init_hook(&(vars->sgg_Hook), hook_routine);
        unlockbasescreen(&ilock, &screen);
	return RUNNING;
}

static int handlekeys(void)
{
	struct IntuiMessage *message;
	int state = RUNNING;
	unsigned char *stribuf = ((struct StringInfo *)dawin->FirstGadget->SpecialInfo)->Buffer;
	int strinc = ((struct StringInfo *)dawin->FirstGadget->SpecialInfo)->NumChars;

	while (NULL != (message = (struct IntuiMessage *)GetMsg(dawin->UserPort))) { //-V2545
		unsigned long class  = message->Class;

		ReplyMsg((struct Message *)message); //-V2545
		switch (class)
		{
		case IDCMP_ACTIVEWINDOW:
			break;
		case IDCMP_MOUSEMOVE:
			break;
		case IDCMP_GADGETDOWN:
			break;
		case IDCMP_GADGETUP:
			if ((sel) && (strinc > 0)) {
				exec_match(stribuf);
			}
			state = DONE;
			break;
	        case IDCMP_VANILLAKEY:
            		break;
		case IDCMP_REFRESHWINDOW:
			BeginRefresh(dawin);
			EndRefresh(dawin, TRUE);
			break;
		default:
			// Default
			break;
        	}
	}
	return state;
}

static int exec_match(unsigned char *em)
{
        struct TagItem stags[6];
        long int file;
	unsigned char conline[FN_MAX_LENGTH];
	unsigned char dem[FN_MAX_LENGTH];

	(void)snprintf((char *)conline, TT_MAX_LENGTH, "%s%s%s", DEFCON_PRE, em, DEFCON_POST);
	if (paths[sel->pathid][strnlen(paths[sel->pathid], FN_MAX_LENGTH)-1U] == ':') {
		(void)snprintf((char *)dem, FN_MAX_LENGTH, "%s%s", paths[sel->pathid], em);
	} else {
		(void)snprintf((char *)dem, FN_MAX_LENGTH, "%s%s%s", paths[sel->pathid], "/", em);
	}

        if ((file = Open(conline, MODE_NEWFILE))) {
                // Will not fix MISRA warnings from amiga NDK
                stags[0].ti_Tag = SYS_Input; //-V2544 //-V2568
                stags[0].ti_Data = (long unsigned int)file;
                stags[1].ti_Tag = SYS_Output; //-V2544 //-V2568
                stags[1].ti_Data = 0; //-V2568
                stags[2].ti_Tag = SYS_Asynch; //-V2544 //-V2568
                stags[2].ti_Data = TRUE; //-V2568
                stags[3].ti_Tag = SYS_UserShell; //-V2544 //-V2568
                stags[3].ti_Data = TRUE; //-V2568
                stags[4].ti_Tag = NP_StackSize; //-V2544 //-V2568
                stags[4].ti_Data = pstack_size; //-V2568
                stags[5].ti_Tag = TAG_DONE; //-V2568

                if ((SystemTagList(dem, stags)) == -1) {
                        return RUNNING;
                }

                return DONE;
        } else {
                return RUNNING;
        }
}

static int match_to_win(unsigned char *strbuf)
{
	struct item *item;
	unsigned char *ptext = NULL;
	short pos = 0;

	(void)snprintf(text, FN_MAX_LENGTH, "%s", (const char *)strbuf);

	if ((match()) == DONE) {
		return DONE;
	}

	RectFill(dawin->RPort, stext.LeftEdge, 0, screen->Width, winh);
	pos = stext.LeftEdge;

	for (item = matches; item && item->text; ptext = (unsigned char *)item->text, item++) {
		short spos = 0;
		struct item *tmpitem = item;
		if (item->text != matches->text) {
			pos = (short)(textwidth(pos, ptext) + (short)spacew);
		}

		mtext.IText = (unsigned char *)item->text;
		mtext.LeftEdge = pos;
		PrintIText(dawin->RPort, &mtext, LOFFS, TOFFS);
		spos = textwidth(pos, (unsigned char *)item->text);
		tmpitem++;
		if (dawin->Width < (textwidth(spos, (unsigned char *)tmpitem->text))) {
			break;
		}
		mtext.IText = space;
		mtext.LeftEdge = spos;
		PrintIText(dawin->RPort, &mtext, LOFFS, TOFFS);
	}
	mtext.LeftEdge = 0;
	return RUNNING;
}

static short int textwidth(int lei, unsigned char * it)
{
        struct TextExtent *barte = &teinit;
        short int le = (short int)lei;

        if (TextExtent(dawin->RPort, it, strnlen((const char *)it, FN_MAX_LENGTH), barte)) {
                if (barte == NULL) {
                        return FALSE;
                }
        }
        return (short int)(le+(short int)barte->te_Width);
}

static void lockbasescreen(unsigned long *il, struct Screen **s)
{
        *il = LockIBase(0L);
        *s = LockPubScreen(NULL);
}

static void unlockbasescreen(unsigned long *il, struct Screen **s)
{
        UnlockPubScreen(NULL, *s);
        UnlockIBase(*il);
}


unsigned long __saveds hookEntry(struct Hook *hookptr __asm("a0"), void  *object __asm("a2"), void  *message __asm("a1"))
{
	return((*hookptr->h_SubEntry)(hookptr, object, message));
}

static void init_hook(struct Hook *hook, unsigned long (*ccode)(struct Hook *hook, struct SGWork *sgw, unsigned long *msg)) {
	hook->h_Entry    = hookEntry;
	hook->h_SubEntry = ccode;
	hook->h_Data     = 0;   /* this program does not use this */
}

static unsigned long hook_routine(__attribute__((unused)) struct Hook *hook, struct SGWork *sgw, unsigned long *msg)
{
	unsigned long return_code;

	return_code = ~0UL;
	if (*msg == (unsigned long)SGH_KEY) {
		if ((sgw->EditOp == REPLACE_C) || (sgw->EditOp == INSERT_C)) {
			if (custom_exec_n == 0) {
				if((match_to_win(sgw->WorkBuffer) == DONE)) {
					Signal(tabexectask, deadsig);
				}
				sel = matches;
			}
			tabc = 0;
			//return return_code;
		}

		switch (sgw->Code) {
		case ESCAPE_C:
			Signal(tabexectask, deadsig);
			break;
		case SPACE_C:
			if (custom_exec_n == 0) {
				custom_exec_n = sgw->BufferPos;
			}
			break;
		case PLUS_C:
			if (custom_exec_n < sgw->BufferPos) {
				custom_exec_n = 0;
			} else {
				break;
			}
			curr++;
			if (curr->text == NULL) {
				curr = matches;
			}
			sel = curr;
			sgw->NumChars = sgw->BufferPos = bufmov(sgw->WorkBuffer, curr->text);
			break;
		case MINUS_C:
			if (custom_exec_n < sgw->BufferPos) {
				custom_exec_n = 0;
			} else {
				break;
			}
			if (--curr != NULL) {
				if ((strnlen(curr->text, FN_MAX_LENGTH)) == 0U) {
					curr = matches;
				}
			}
			sel = curr;
			sgw->NumChars = sgw->BufferPos = bufmov(sgw->WorkBuffer, curr->text);
			break;
		case TAB_C:
			if (custom_exec_n < sgw->BufferPos) {
				custom_exec_n = 0;
			} else {
				break;
			}
			if (sgw->NumChars > 0) {
				if (tabc == 0) {
					tabc++;
					sel = matches;
					sgw->NumChars = sgw->BufferPos = bufmov(sgw->WorkBuffer, matches->text);
				} else {
					curr++;
					if (curr->text == NULL) {
						curr = matches;
					}
					sel = curr;
					sgw->NumChars = sgw->BufferPos = bufmov(sgw->WorkBuffer, curr->text);
				}
			}
			break;
		case BACKSPACE_C:
			tabc = 0;
			if (custom_exec_n < sgw->BufferPos) {
				custom_exec_n = 0;
			}

			if (sgw->BufferPos == 0) {
				RectFill(dawin->RPort, stext.LeftEdge, 0, screen->Width, winh);
				sel = NULL;
			} else {
				if((match_to_win(sgw->WorkBuffer) == DONE)) {
					Signal(tabexectask, deadsig);
				}
				sel = matches;
			}
			break;
		default:
			// Do nothing
			break;
		}
	} else {
		// UNKNOWN COMMAND
		return_code = 0UL;
	}

	return return_code;
}

short bufmov(unsigned char *wb, char *s)
{
	return (short)snprintf((char *)wb, FN_MAX_LENGTH, "%s", (const char *)s);
}

static void appenditem(struct item *item, struct item **list, struct item **last)
{
        if (*last) {
		(*last)->right = item;
	} else {
		*list = item;
	}

	item->left = *last;
	item->right = NULL;
	*last = item;
}

static int match(void)
{
        static char **tokv = NULL;
        static char **toktmp = NULL;
        static int tokn = 0;

        char buf[sizeof text], *s;
        int i, tokc = 0;
        size_t len, textsize;
        struct item *item, *lprefix, *lsubstr, *prefixend, *substrend;

	(void)snprintf(buf, FN_MAX_LENGTH, "%s", text);

        /* separate input text into tokens to be matched individually */
        for (s = strtok(buf, " "); s; tokv[tokc - 1] = s, s = strtok(NULL, " ")) {
                if (++tokc > tokn && !(toktmp = realloc(tokv, (unsigned int)++tokn * (unsigned int)sizeof *tokv))) {
			free(toktmp);
			return DONE;
		} else {
			tokv = toktmp;
		}
	}

        len = (size_t)tokc != 0U ? strnlen(tokv[0], BUFFERSIZE) : 0U;

        matches = lprefix = lsubstr = matchend = prefixend = substrend = NULL;
        textsize = strnlen(text, FN_MAX_LENGTH) + 1U;
        for (item = items; item && item->text; item++) {
                for (i = 0; i < tokc; i++) {
                        if (!fstrstr(item->text, tokv[i])) {
                                break;
			}
		}
                if (i != tokc) { /* not all tokens match */
                        continue;
		}
                /* exact matches go first, then prefixes, then substrings */
                if (tokc == 0 || (fstrncmp(text, item->text, textsize) == 0)) {
                        appenditem(item, &matches, &matchend);
		} else if ((fstrncmp(tokv[0], item->text, len) == 0)) {
                        appenditem(item, &lprefix, &prefixend);
		} else {
                        appenditem(item, &lsubstr, &substrend);
		}
        }
        if (lprefix) {
                if (matches) {
                        matchend->right = lprefix;
                        lprefix->left = matchend;
                } else {
                        matches = lprefix;
		}
                matchend = prefixend;
        }
        if (lsubstr) {
                if (matches) {
                        matchend->right = lsubstr;
                        lsubstr->left = matchend;
                } else {
                        matches = lsubstr;
		}
                matchend = substrend;
        }
        curr = matches;
	return RUNNING;
}

static void freetext(void)
{
	for (size_t i = 0; i < (sizeof(*items)); i++) {
		free(&items[i].text);
		free(&items[i]);
	}
	free(items);
}

static size_t strnlen(const char *s, size_t maxlen)
{
        size_t len;

        for (len = (size_t)0; len < maxlen; len++, s++) {
                if (*s == '\0') {
                        break;
                }
        }
        return len;
}

static void cleanup(void)
{
	free(freepaths);
	freetext();
	CloseWindow(dawin);
	FreeSignal((long)deadsignum);
	free(vars);
	FreeScreenDrawInfo(screen, drawinfo);
}
