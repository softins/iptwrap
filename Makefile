#-----------------------------------------------------------------------------
# Copyright (C) 2012, Tony Mountifield
#-----------------------------------------------------------------------------
#
# File:		Makefile
#
# Description:	Makefile for IPtables wrapper
#
# Author:	Tony Mountifield
# Date:		12-Nov-2012
#
#-----------------------------------------------------------------------------

CC = gcc
INCLUDES = 
WRAPSRCS = iptwrap.c cmd.c
WRAPOBJS = $(WRAPSRCS:.c=.o)
WRAPDEPS = $(WRAPSRCS:.c=.d)
WRAPPROG = iptwrap

BINDIR=/usr/local/bin

CFLAGS = -Wall -Werror -g -O2 -MMD -MP $(INCLUDES) -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE
LFLAGS = -Wl,-Map,$@.map -Wl,--cref

all: $(WRAPPROG)

-include $(WRAPDEPS)

$(WRAPPROG): $(WRAPOBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $(WRAPOBJS) $(LIBS)
	@echo "NB: $@ needs to be installed suid root"

clean:
	rm -f $(WRAPPROG) $(WRAPPROG).map $(WRAPOBJS) $(WRAPDEPS)

install: all
	install -m 4755 $(WRAPPROG) $(BINDIR)
