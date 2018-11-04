#ifndef _TF_CIPHER_SUPPORTING_HEADER
#define _TF_CIPHER_SUPPORTING_HEADER

#include <stddef.h>
#include <stdio.h>

typedef unsigned long long tf_fsize;

typedef void (*sighandler_t)(int);

#define NOSIZE ((size_t)-1)
#define NOFSIZE ((tf_fsize)-1)

#define DATASIZE 65536
#define TESTTIME 5

#define HD_WIDTH 32

#define newline() do { putc('\n', stdout); } while (0)
size_t xstrlcpy(char *dst, const char *src, size_t size);
#define xzstrlcpy(d, s, sz) do { memset(d, 0, sz); xstrlcpy(d, s, sz); } while(0)
int mhexdump(const void *data, size_t szdata, int hgroup);
size_t blk_len_adjust(tf_fsize filelen, tf_fsize read_already, size_t blklen);
tf_fsize tf_humanfsize(const char *s, char **stoi);

#endif
