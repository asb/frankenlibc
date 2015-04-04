#include <string.h>

void *memcpy(void *, const void *, size_t) __attribute ((weak));

void *
memcpy(void *dest, const void *src, size_t n)
{
	char *d = dest;
	const char *s = src;

	while (n--)
		*d++ = *s++;

	return dest;
}
