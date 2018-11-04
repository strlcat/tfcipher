#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "tfdef.h"
#include "tfsupport.h"

size_t blk_len_adjust(tf_fsize filelen, tf_fsize read_already, size_t blklen)
{
	if (filelen == NOFSIZE) return blklen;
	return ((filelen - read_already) >= blklen) ? blklen : (filelen - read_already);
}

static int str_empty(const char *str)
{
	if (!*str) return 1;
	return 0;
}

static int is_number(const char *s)
{
	char *p;
	if (!s || str_empty(s)) return 0;
	strtol(s, &p, 10);
	return str_empty(p) ? 1 : 0;
}

tf_fsize tf_humanfsize(const char *s, char **stoi)
{
	char pfx[2], N[48];
	int base = 10;
	size_t l;
	tf_fsize mult = 0, ret = 0;

	if (!s) return 0;

	memset(N, 0, sizeof(N));
	memset(pfx, 0, sizeof(pfx));

	if (!strncmp(s, "0x", 2)) {
		s += 2;
		base = 16;
	}
	else if (s[0] == '0') base = 0;

	l = strnlen(s, sizeof(N)-1);
	memcpy(N, s, l);

	if (base == 16) goto _nopfx;

	pfx[0] = N[l-1];
	if (!is_number(pfx)) N[l-1] = 0;

_nopfx:
	*stoi = NULL;
	if (is_number(pfx) || pfx[0] == 'B' || pfx[0] == 'c') ret = strtoull(N, stoi, base);
	else if (pfx[0] == 'W') ret = strtoull(N, stoi, base)*2;
	else if (pfx[0] == 'I') ret = strtoull(N, stoi, base)*4;
	else if (pfx[0] == 'L') ret = strtoull(N, stoi, base)*8;
	else if (pfx[0] == 'e') ret = strtoull(N, stoi, base)*TF_BLOCK_SIZE;
	else if (pfx[0] == 'y') ret = strtoull(N, stoi, base)*TF_KEY_SIZE;
	else if (pfx[0] == 'x') ret = strtoull(N, stoi, base)*TF_KEY_SIZE*2;
	else if (pfx[0] == 'b' || pfx[0] == 's') ret = strtoull(N, stoi, base)*512;
	else if (pfx[0] == 'p' || pfx[0] == 'S') ret = strtoull(N, stoi, base)*4096;
	else if (pfx[0] == 'k' || pfx[0] == 'K') mult = 1024;
	else if (pfx[0] == 'm' || pfx[0] == 'M') mult = 1024 * 1024;
	else if (pfx[0] == 'g' || pfx[0] == 'G') mult = 1024 * 1024 * 1024;
	else if (pfx[0] == 'T') mult = 1099511627776ULL;
	else if (pfx[0] == 'P') mult = 1125899906842624ULL;
	else ret = strtoull(N, stoi, base);
	if (mult) ret = strtoull(N, stoi, base) * mult;

	return ret;
}
