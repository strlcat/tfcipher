/*
 * mhexdump.c - a memory block embeddable hexdump(1).
 * You call mhexdump(p, n) and output is a nice hexdump(1) compatible print.
 *
 * -- Lynx, Jun2016.
 */

#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#undef MACHINE_16BIT
#undef MACHINE_32BIT
#undef MACHINE_64BIT

#if UINTPTR_MAX == UINT32_MAX
#define MACHINE_32BIT
#elif UINTPTR_MAX == UINT64_MAX
#define MACHINE_64BIT
#elif UINTPTR_MAX == UINT16_MAX
#define MACHINE_16BIT
#endif

struct mhexdump_args {
	const void *data;
	size_t szdata;
	int group;
	int hexgroup;
	int hexstr;
	int addaddr;
	int newline;
	FILE *fp;
	int closef;
};

#if defined(MACHINE_32BIT)
#define ADDRFMT "%08x: "
#define paddr (mha->addaddr == 2 ? (uint32_t)P+(x*mha->group) : (x*mha->group))
#elif defined(MACHINE_64BIT)
#define ADDRFMT "%016lx: "
#define paddr (mha->addaddr == 2 ? (uint64_t)P+(x*mha->group) : (x*mha->group))
#elif defined(MACHINE_16BIT)
#define ADDRFMT "%04x: "
#define paddr (mha->addaddr == 2 ? (uint16_t)P+(x*mha->group) : (x*mha->group))
#else
#error No machine word size detected!
#endif

#define BYTEOUT ((unsigned char)P[y+(x*mha->group)])

int fmhexdump(const struct mhexdump_args *mha)
{
	const unsigned char *P = (unsigned char *)mha->data;
	int x, y;

	if (!mha->fp || !mha->data || mha->szdata == 0) return 0;

	for (x = 0; x < mha->szdata/mha->group; x++) {
		if (mha->addaddr) fprintf(mha->fp, ADDRFMT, paddr);
		for (y = 0; y < mha->group; y++) {
			fprintf(mha->fp, "%02hhx", BYTEOUT);
			if (((y+1) % mha->hexgroup) == 0 && (y != (mha->group)-1)) fputc(' ', mha->fp);
		}
		if (mha->hexstr) fprintf(mha->fp, "  ");
		if (mha->hexstr) for (y = 0; y < mha->group; y++) {
			if (isprint(BYTEOUT)) fprintf(mha->fp, "%c", BYTEOUT);
			else fputc('.', mha->fp);
		}
		if (mha->szdata/mha->group == 1 && mha->szdata-mha->group == 0) {
			if (mha->newline) fputc('\n', mha->fp);
		}
		else fputc('\n', mha->fp);
	}
	if (mha->szdata-(x*mha->group) == 0) goto _ret;

	if (mha->addaddr) fprintf(mha->fp, ADDRFMT, paddr);
	for (y = 0; y < mha->szdata-(x*mha->group); y++) {
		fprintf(mha->fp, "%02hhx", BYTEOUT);
		if (((y+1) % mha->hexgroup) == 0) fputc(' ', mha->fp);
	}
	if (mha->hexstr) for (; y < mha->group; y++) {
		fprintf(mha->fp, "  ");
		if (((y+1) % mha->hexgroup) == 0 && (y != mha->group-1)) fputc(' ', mha->fp);
	}
	if (mha->hexstr) fprintf(mha->fp, "  ");
	if (mha->hexstr) for (y = 0; y < mha->szdata-(x*mha->group); y++) {
		if (isprint(BYTEOUT)) fprintf(mha->fp, "%c", BYTEOUT);
		else fputc('.', mha->fp);
	}

	if (mha->newline) fputc('\n', mha->fp);

_ret:	fflush(mha->fp);
	if (mha->closef) fclose(mha->fp);
	return 1;
}

#undef BYTEOUT

int mhexdump(const void *data, size_t szdata, int hgroup)
{
	struct mhexdump_args mha;

	if (hgroup == 0) hgroup = 16;

	memset(&mha, 0, sizeof(struct mhexdump_args));
	mha.fp = stdout;
	mha.closef = 0;
	mha.data = data;
	mha.szdata = szdata;
	mha.group = hgroup;
	mha.hexgroup = hgroup;
	mha.hexstr = 1;
	mha.addaddr = 0;
	mha.newline = 1;

	return fmhexdump(&mha);
}
