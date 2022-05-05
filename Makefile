define newline


endef

SOURCES = src/tabexec.c
OBJECTS = $(SOURCES:.c=.o)
CFLAGSSTRICT =-fbaserel -pedantic -std=c11 -ggdb3 -O0 -Wall -Wextra -Wformat=2 -Wmissing-include-dirs -Winit-self -Wswitch-default -Wswitch-enum -Wunused-parameter -Wfloat-equal -Wundef -Wshadow -Wlarger-than-1000 -Wunsafe-loop-optimizations -Wbad-function-cast -Wcast-align -Wconversion -Wlogical-op -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wpacked -Wpadded -Wredundant-decls -Wnested-externs -Wunreachable-code -Winline -Winvalid-pch -Wvolatile-register-var -Wstrict-aliasing=2 -Wstrict-overflow=2 -Wtraditional-conversion -Wwrite-strings -noixemul -save-temps=obj
CC = m68k-amigaos-gcc
CFLAGS =-std=c11 -Os -noixemul -fbaserel -Wall -Wextra -s -fomit-frame-pointer
MAINHEADER = include/tabexec.h
HEADERS = $(MAINHEADER)
CPPLINTCMD = cpplint
CPPLINTOPTS = --linelength 150 --filter=-whitespace/tab,-whitespace/comments,-whitespace/braces,-whitespace/indent,-readability/casting,-runtime/int,-build/header_guard,-runtime/threadsafe_fn,-runtime/printf
CPPCHECKCMD = cppcheck
CPPCHECKOPTS = --enable=warning,style,performance,portability,unusedFunction
FLAWCMD = flawfinder
FLAWOPTS = -F
SPLINTCMD = splint
SPLINTARGS = -I $(HOME)/opt/amiga/m68k-amigaos/ndk-include/
PANDOC = cat amiheader.txt > tabexec.readme && sed 's/\[\/\/\].*/--/g' README.md | pandoc -f markdown -t plain - >> tabexec.readme
TARGET = tabexec

ifdef strict
CFLAGS = $(CFLAGSSTRICT)
TEMPS = $(SOURCES:.c=.i) $(SOURCES:.c=.s)
LOGS = $(SOURCES:.c=.log)
PEXCL = --exclude-path */opt/amiga/*
ifdef misra
MPLOGFLAG = -d V2511,V2516,V2510 -a MISRA:1,2
endif
endif

.PHONY: setver
setver:
	$(VERCMD)

all : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
ifdef strict
	$(PANDOC)
	$(CPPLINTCMD) $(CPPLINTOPTS) $(SOURCES)
	$(CPPCHECKCMD) $(CPPCHECKOPTS) $(HEADERS) $(SOURCES)
	$(FLAWCMD) $(FLAWOPTS) $(HEADERS) $(SOURCES)
	$(SPLINTCMD) $(SOURCES) $(SPLINTARGS)
	$(foreach elem,$(SOURCES),pvs-studio --cfg PVS-Studio.cfg $(PEXCL) --source-file $(elem) --i-file $(elem:.c=.i) --output-file $(elem:.c=.log)${newline})
ifdef misra
	$(foreach elem,$(SOURCES),plog-converter $(MPLOGFLAG) -t csv $(elem:.c=.log) | grep 'Filtered' ${newline})
	$(foreach elem,$(SOURCES),plog-converter -t csv $(elem:.c=.log) | grep 'Filtered' ${newline})
endif
endif

.PHONY : clean
clean :
	-rm tabexec $(OBJECTS) $(TEMPS) $(LOGS)

