COMPILE.c?=$(CC) -c $(CPPFLAGS) $(CFLAGS)
COMPILE.S?=$(CC) -c $(CPPFLAGS) $(ASFLAGS)
RUMPOBJ?=../../rumpobj
OBJDIR=$(RUMPOBJ)/platform
RUMP?=$(RUMPOBJ)/rump
CRTDIR=$(RUMP)/lib
OBJ=$(CRTDIR)/crt1.o $(CRTDIR)/Scrt1.o
OBJ+=$(CRTDIR)/crti.o $(CRTDIR)/crtn.o
OBJ+=$(CRTDIR)/crtbegin.o $(CRTDIR)/crtbeginT.o
OBJ+=$(CRTDIR)/crtend.o
OBJ+=$(OBJDIR)/clock_getres.o
OBJ+=$(OBJDIR)/clock_gettime.o
OBJ+=$(OBJDIR)/clock_nanosleep.o
OBJ+=$(OBJDIR)/_exit.o
OBJ+=$(OBJDIR)/fcntl.o
OBJ+=$(OBJDIR)/fstat.o
OBJ+=$(OBJDIR)/fsync.o
OBJ+=$(OBJDIR)/getpagesize.o
OBJ+=$(OBJDIR)/getrandom.o
OBJ+=$(OBJDIR)/kill.o
OBJ+=$(OBJDIR)/mmap.o
OBJ+=$(OBJDIR)/mprotect.o
OBJ+=$(OBJDIR)/munmap.o
OBJ+=$(OBJDIR)/__libc_start_main.o
OBJ+=$(OBJDIR)/pread.o
OBJ+=$(OBJDIR)/preadv.o
OBJ+=$(OBJDIR)/pwrite.o
OBJ+=$(OBJDIR)/pwritev.o
OBJ+=$(OBJDIR)/read.o
OBJ+=$(OBJDIR)/readv.o
OBJ+=$(OBJDIR)/set_thread_area.o
OBJ+=$(OBJDIR)/write.o
OBJ+=$(OBJDIR)/writev.o
OBJ+=$(OBJDIR)/syscall.o $(OBJDIR)/syscall_ret.o
OBJ+=$(OBJDIR)/vdso.o
HEADERS=syscall.h linux.h
INCLUDES=-I../include -I../../franken/include -I../../franken/init -I../../franken/ucontext -I../../librumpuser -I../../franken/thread -I.
CPPFLAGS+=-nostdinc $(INCLUDES)
CFLAGS+=-fno-stack-protector

default:			$(CRTDIR) $(OBJDIR) $(OBJ)

$(OBJDIR):			
				mkdir -p $(OBJDIR)

$(CRTDIR):			
				mkdir -p $(CRTDIR)

$(CRTDIR)/crt1.o:		crt1.c $(CRTDIR)
				$(COMPILE.c) -o $@ crt1.c

$(CRTDIR)/Scrt1.o:		$(CRTDIR)/crt1.o
				ln -f $(CRTDIR)/crt1.o $@

$(CRTDIR)/crti.o:		$(CRTDIR)
				printf "\n" | $(COMPILE.c) -o $@ -x c -

$(CRTDIR)/crtn.o:		$(CRTDIR)
				printf "\n" | $(COMPILE.c) -o $@ -x c -

$(CRTDIR)/crtbegin.o:		$(CRTDIR)
				printf "\n" | $(COMPILE.c) -o $@ -x c -

$(CRTDIR)/crtbeginT.o:		$(CRTDIR)
				printf "\n" | $(COMPILE.c) -o $@ -x c -

$(CRTDIR)/crtend.o:		$(CRTDIR)
				printf "\n" | $(COMPILE.c) -o $@ -x c -

$(OBJDIR)/clock_getres.o:	clock_getres.c $(HEADERS)
				$(COMPILE.c) -o $@ clock_getres.c

$(OBJDIR)/clock_gettime.o:	clock_gettime.c $(HEADERS)
				$(COMPILE.c) -o $@ clock_gettime.c

$(OBJDIR)/clock_nanosleep.o:	clock_nanosleep.c $(HEADERS)
				$(COMPILE.c) -o $@ clock_nanosleep.c

$(OBJDIR)/_exit.o:		_exit.c $(HEADERS)
				$(COMPILE.c) -o $@ _exit.c

$(OBJDIR)/fcntl.o:		fcntl.c $(HEADERS)
				$(COMPILE.c) -o $@ fcntl.c

$(OBJDIR)/fstat.o:		fstat.c $(HEADERS)
				$(COMPILE.c) -o $@ fstat.c

$(OBJDIR)/fsync.o:		fsync.c $(HEADERS)
				$(COMPILE.c) -o $@ fsync.c

$(OBJDIR)/getpagesize.o:	getpagesize.c $(HEADERS)
				$(COMPILE.c) -o $@ getpagesize.c

$(OBJDIR)/getrandom.o:		getrandom.c $(HEADERS)
				$(COMPILE.c) -o $@ getrandom.c

$(OBJDIR)/kill.o:		kill.c $(HEADERS)
				$(COMPILE.c) -o $@ kill.c

$(OBJDIR)/__libc_start_main.o:	__libc_start_main.c $(HEADERS)
				$(COMPILE.c) -o $@ __libc_start_main.c

$(OBJDIR)/mmap.o:		mmap.c $(HEADERS)
				$(COMPILE.c) -o $@ mmap.c

$(OBJDIR)/mprotect.o:		mprotect.c
				$(COMPILE.c) -o $@ mprotect.c

$(OBJDIR)/munmap.o:		munmap.c $(HEADERS)
				$(COMPILE.c) -o $@ munmap.c

$(OBJDIR)/pread.o:		pread.c $(HEADERS)
				$(COMPILE.c) -o $@ pread.c

$(OBJDIR)/preadv.o:		preadv.c $(HEADERS)
				$(COMPILE.c) -o $@ preadv.c

$(OBJDIR)/pwrite.o:		pwrite.c $(HEADERS)
				$(COMPILE.c) -o $@ pwrite.c

$(OBJDIR)/pwritev.o:		pwritev.c $(HEADERS)
				$(COMPILE.c) -o $@ pwritev.c

$(OBJDIR)/read.o:		read.c $(HEADERS)
				$(COMPILE.c) -o $@ read.c

$(OBJDIR)/readv.o:		readv.c $(HEADERS)
				$(COMPILE.c) -o $@ readv.c

$(OBJDIR)/set_thread_area.o:	set_thread_area.c $(HEADERS)
				$(COMPILE.c) -o $@ set_thread_area.c

$(OBJDIR)/syscall.o:		syscall.S $(HEADERS)
				$(COMPILE.S) -o $@ syscall.S

$(OBJDIR)/syscall_ret.o:	syscall_ret.c $(HEADERS)
				$(COMPILE.c) -o $@ syscall_ret.c

$(OBJDIR)/vdso.o:		vdso.c $(HEADERS)
				$(COMPILE.c) -o $@ vdso.c

$(OBJDIR)/write.o:		write.c $(HEADERS)
				$(COMPILE.c) -o $@ write.c

$(OBJDIR)/writev.o:		writev.c $(HEADERS)
				$(COMPILE.c) -o $@ writev.c

.PHONY:				clean
clean:		
				rm -f $(OBJDIR)/*.o $(CRTDIR)/*.o
