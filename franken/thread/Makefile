COMPILE.c?=$(CC) -c $(CPPFLAGS) $(CFLAGS)
COMPILE.S?=$(CC) -c $(CPPFLAGS) $(ASFLAGS)
RUMPOBJ?=../../rumpobj
OBJDIR=$(RUMPOBJ)/franken
OBJ=$(OBJDIR)/thread.o
OBJ+=$(OBJDIR)/errno.o
HEADERS=thread.h
INCLUDES=-I../include -I../ucontext -I../../librumpuser -I../../platform/include
CPPFLAGS+=-nostdinc $(INCLUDES)
CFLAGS+=-fno-stack-protector
DEPS=$(HEADERS) $(OBJDIR)

default:		$(OBJ)

$(OBJDIR):
			mkdir -p $(OBJDIR)

$(OBJDIR)/errno.o:	errno.c $(DEPS)
			$(COMPILE.c) -o $@ errno.c

$(OBJDIR)/thread.o:	thread.c $(DEPS)
			$(COMPILE.c) -o $@ thread.c

.PHONY:			clean
clean:			
			rm -f $(OBJ)
