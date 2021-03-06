#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <linux/fs.h>
#include <net/if.h>
#include <linux/if_tun.h>

#include "rexec.h"

/* only in linux/fcntl.h that cannot be included */
#ifndef AT_EMPTY_PATH
#define AT_EMPTY_PATH		0x1000
#endif

#ifndef SYS_getrandom
#if defined(__x86_64__)
#define SYS_getrandom 318
#elif defined(__i386__)
#define SYS_getrandom 355
#elif defined(__ARMEL__) || defined(__ARMEB__)
#define SYS_getrandom 384
#elif defined(__AARCH64EL__) || defined (__AARCH64EB__)
#define SYS_getrandom 278
#elif defined(__PPC64__)
#define SYS_getrandom 359
#elif defined(__PPC__)
#define SYS_getrandom 359
#elif defined(__MIPSEL__) || defined(__MIPSEB__)
#define SYS_getrandom 4353
#else
#error "Unknown architecture"
#endif
#endif

#ifndef SECCOMP
int
filter_init(char *program)
{
	int ret;

	return 0;
}

int
filter_fd(int fd, int flags, struct stat *st)
{

	return 0;
}

int
filter_load_exec(char *program, char **argv, char **envp)
{
	int ret;

	if (execve(program, argv, envp) == -1) {
		perror("execve");
		exit(1);
	}

	return 0;
}
#else

#include <seccomp.h>

#ifdef SYS_arch_prctl
#include <asm/prctl.h>
#include <sys/prctl.h>
#endif

scmp_filter_ctx ctx;

int
filter_init(char *program)
{
	int ret, i;

	ctx = seccomp_init(SCMP_ACT_KILL);
	if (ctx == NULL)
		return -1;

	/* arch_prctl(ARCH_SET_FS, x) */
#ifdef SYS_arch_prctl
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(arch_prctl), 1,
		SCMP_A0(SCMP_CMP_EQ, ARCH_SET_FS));
	if (ret < 0) return ret;
#endif

	/* exit(x) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
	if (ret < 0) return ret;

	/* exit_group(x) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);
	if (ret < 0) return ret;

	/* clock_gettime(x, y) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clock_gettime), 0);
	if (ret < 0) return ret;

	/* clock_getres(x, y) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clock_getres), 0);
	if (ret < 0) return ret;

	/* clock_nanosleep(w, x, y, z) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clock_nanosleep), 0);
	if (ret < 0) return ret;

	/* getrandom(x, y, z) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SYS_getrandom, 0);
	if (ret < 0) return ret;

	/* kill(0, SIGABRT) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(kill), 2,
		SCMP_A0(SCMP_CMP_EQ, 0), SCMP_A1(SCMP_CMP_EQ, SIGABRT));
	if (ret < 0) return ret;

	/* mmap(a, b, c, d, -1, e) */
#ifdef SYS_mmap2
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap2), 0);
#else
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
#endif
	if (ret < 0) return ret;

	/* mprotect(a, b, c) XXX allow disable exec */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 0);
	if (ret < 0) return ret;

	/* munmap(a, b) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0);
	if (ret < 0) return ret;

	/* fstat(a, b) as used to check for existence */
#ifdef SYS_fstat64
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat64), 0);
#else
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat), 0);
#endif
	if (ret < 0) return ret;

	/* allow ppoll for network readiness */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(ppoll), 0);
	if (ret < 0) return ret;

	return 0;
}

int
filter_fd(int fd, int flags, struct stat *st)
{
	int ret;

	/* read(fd, ...), pread(fd, ...) */
	if ((flags & O_ACCMODE) == O_RDONLY || (flags & O_ACCMODE) == O_RDWR) {
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 1,
			SCMP_A0(SCMP_CMP_EQ, fd));
		if (ret < 0) return ret;
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(pread64), 1,
			SCMP_A0(SCMP_CMP_EQ, fd));
		if (ret < 0) return ret;
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(readv), 1,
			SCMP_A0(SCMP_CMP_EQ, fd));
		if (ret < 0) return ret;
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(preadv), 1,
			SCMP_A0(SCMP_CMP_EQ, fd));
	}

	/* write(fd, ...), pwrite(fd, ...) */
	if ((flags & O_ACCMODE) == O_WRONLY || (flags & O_ACCMODE) == O_RDWR) {
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 1,
			SCMP_A0(SCMP_CMP_EQ, fd));
		if (ret < 0) return ret;
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(pwrite64), 1,
			SCMP_A0(SCMP_CMP_EQ, fd));
		if (ret < 0) return ret;
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(writev), 1,
			SCMP_A0(SCMP_CMP_EQ, fd));
		if (ret < 0) return ret;
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(pwritev), 1,
			SCMP_A0(SCMP_CMP_EQ, fd));
	}

	/* fsync(fd) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fsync), 1,
		SCMP_A0(SCMP_CMP_EQ, fd));
	if (ret < 0) return ret;

	/* fcntl(fd, F_GETFL, ...) */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fcntl), 2,
		SCMP_A0(SCMP_CMP_EQ, fd), SCMP_A1(SCMP_CMP_EQ, F_GETFL));
	if (ret < 0) return ret;

	/* ioctl(fd, BLKGETSIZE64) for block devices */
	if (S_ISBLK(st->st_mode)) {
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(ioctl), 2,
			SCMP_A0(SCMP_CMP_EQ, fd), SCMP_A1(SCMP_CMP_EQ, BLKGETSIZE64));
		if (ret < 0) return ret;
	}

	/* XXX be more specific, only for tap devices */
	if (S_ISCHR(st->st_mode)) {
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(ioctl), 2,
			SCMP_A0(SCMP_CMP_EQ, fd), SCMP_A1(SCMP_CMP_EQ, TUNGETIFF));
		if (ret < 0) return ret;
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(ioctl), 2,
			SCMP_A0(SCMP_CMP_EQ, fd), SCMP_A1(SCMP_CMP_EQ, TUNSETIFF));
		if (ret < 0) return ret;
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(ioctl), 2,
			SCMP_A0(SCMP_CMP_EQ, fd), SCMP_A1(SCMP_CMP_EQ, SIOCGIFHWADDR));
		if (ret < 0) return ret;
	}
	/* XXX be more specific only for our dummy socket */
	if (S_ISSOCK(st->st_mode)) {
		ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(ioctl), 2,
			SCMP_A0(SCMP_CMP_EQ, fd), SCMP_A1(SCMP_CMP_EQ, SIOCGIFHWADDR));
		if (ret < 0) return ret;
	}

	return 0;
}

/* without being able to exec a file descriptor, we cannot lock down as
   much, but this is only a very recent facility in Linux */
#ifdef SYS_execveat
/* not yet defined by libc */
int execveat(int, const char *, char *const [], char *const [], int);

int
execveat(int dirfd, const char *pathname,
	char *const argv[], char *const envp[], int flags)
{

	return syscall(SYS_execveat, dirfd, pathname, argv, envp, flags);
}

int
filter_load_exec(char *program, char **argv, char **envp)
{
	int ret;
	const char *emptystring = "";
	int fd = open(program, O_RDONLY | O_CLOEXEC);

	if (fd == -1) {
		perror("open");
		exit(1);
	}

	/* lock down execveat to exactly what we need to exec program */
	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execveat), 5,
		SCMP_A0(SCMP_CMP_EQ, fd),
		SCMP_A1(SCMP_CMP_EQ, (long)emptystring),
		SCMP_A2(SCMP_CMP_EQ, (long)argv),
		SCMP_A3(SCMP_CMP_EQ, (long)envp),
		SCMP_A4(SCMP_CMP_EQ, AT_EMPTY_PATH));
	if (ret < 0) return ret;
	ret = emptydir();
	if (ret < 0) return ret;

	/* seccomp_export_pfc(ctx, 1); */

	ret = seccomp_load(ctx);
	if (ret < 0) return ret;

	seccomp_release(ctx);

	if (execveat(fd, emptystring, argv, envp, AT_EMPTY_PATH) == -1) {
		perror("execveat");
		exit(1);
	}

	return 0;
}
#else /* execveat */
int
filter_load_exec(char *program, char **argv, char **envp)
{
	int ret;

	ret = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 0);
	if (ret < 0) return ret;

	/* seccomp_export_pfc(ctx, 1); */

	ret = seccomp_load(ctx);
	if (ret < 0) return ret;

	seccomp_release(ctx);

	if (execve(program, argv, envp) == -1) {
		perror("execve");
		exit(1);
	}

	return 0;
}
#endif /* execveat */

#endif /* SECCOMP */

int getrandom(void *, size_t, unsigned int);

int
getrandom(void *buf, size_t buflen, unsigned int flags)
{

	return syscall(SYS_getrandom, buf, buflen, flags);
}

int
os_pre()
{
	int sock[2];
	int ret, fd;
	char buf[1];

	/* if getrandom() syscall works we do not need to pass random source in */
	if (getrandom(buf, 1, 0) == -1 && errno == ENOSYS) {
		fd = open("/dev/urandom", O_RDONLY);
	}

	/* Linux needs a socket to do ioctls on to get mac addresses */
	/* XXX Add a tap ioctl to get hwaddr */
	ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sock);
	close(sock[1]);

	return ret;
}

int
os_open(char *pre, char *post)
{

	/* eg tun:tap0 for tap0 */
	if (strcmp(pre, "tun") == 0) {
		struct ifreq ifr;
		int fd;

		strncpy(ifr.ifr_name, post, IFNAMSIZ);
		ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

		fd = open("/dev/net/tun", O_RDWR, O_NONBLOCK);
		if (fd == -1)
			return -1;

		if (ioctl(fd, TUNSETIFF, &ifr) == -1)
			return -1;

		return fd;
	}

	fprintf(stderr, "platform does not support %s:%s\n", pre, post);
	exit(1);
}
