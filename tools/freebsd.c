#include <sys/capability.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "filter.h"

int pfd = -1;

int
filter_init(char *program)
{
	cap_rights_t rights;
	int ret;

	pfd = open(program, O_EXEC | O_CLOEXEC);
	if (pfd == -1) {
		perror("open");
		exit(1);
	}

	if (cap_enter() == -1) {
		perror("cap_enter");
		exit(1);
	}

	return 0;
}

int
filter_fd(int fd, int flags, mode_t mode)
{
	cap_rights_t rights;
	int ret;
	unsigned long ioctl[1] = {TIOCGETA};

	/* XXX filter exact ioctl */
	switch (flags) {
	case O_RDONLY:
		cap_rights_init(&rights, CAP_READ, CAP_FSTAT, CAP_IOCTL, CAP_MMAP_R);
		break;
	case O_WRONLY:
		cap_rights_init(&rights, CAP_WRITE, CAP_FSTAT, CAP_IOCTL, CAP_MMAP_W);
		break;
	case O_RDWR:
		cap_rights_init(&rights, CAP_READ, CAP_WRITE, CAP_FSTAT, CAP_IOCTL, CAP_MMAP_RW);
		break;
	default:
		abort();
	}
	ret = cap_rights_limit(fd, &rights);
	if (ret == -1) return ret;

	ret = cap_ioctls_limit(fd, ioctl, 1);
	if (ret == -1) return ret;

	return 0;
}

int
filter_load_exec(char *program, char **argv, char **envp)
{

	if (fexecve(pfd, argv, envp) == -1) {
		perror("fexecve");
		exit(1);
	}

	return 0;
}
