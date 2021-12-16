#include <string.h>

#ifdef __cplusplus
  extern "C" {
#endif

size_t xstrlcpy(char *dst, const char *src, size_t size)
{
	size_t len, srclen;
	srclen = strlen(src);
	if (size-- <= 0) return srclen;
	len = (size < srclen) ? size : srclen;
	memmove(dst, src, len);
	dst[len] = '\0';
	return srclen;
}

#ifdef __cplusplus
  }
#endif
