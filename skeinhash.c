#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "tfdef.h"
#include "skein.h"
#include "tfsupport.h"

static char srcblk[DATASIZE];
static char dgst[SKEIN_DIGEST_SIZE];
static int will_exit;

static struct skein sk;

static void usage(void)
{
	printf("usage: skeinhash [-b BITS] srcfile.\n");
	printf("This program is part of Threefish cipher reference.\n");
	exit(1);
}

static void xerror(const char *s)
{
	perror(s);
	exit(2);
}

int main(int argc, char **argv)
{
	int fd;
	char *fname;
	size_t lio, lrem, ldone, lblock;
	size_t bits = TF_MAX_BITS;
	char *pblk;

	fname = argv[1];

	if (argc < 2) usage();
	if (!fname) usage();

	if (!strcmp(fname, "-b") && argc >= 4) {
		bits = atoi(argv[2]);
		if (bits > TF_MAX_BITS) bits = TF_MAX_BITS;
		fname = argv[3];
	}

	if (!strcmp(fname, "-")) fd = 0;
	else {
		fd = open(fname, O_RDONLY);
		if (fd == -1) xerror(fname);
	}

	skein_init(&sk, bits);

	while (1) {
		if (will_exit) break;
		pblk = srcblk;
		ldone = 0;
		lrem = lblock = sizeof(srcblk);
_ragain:	lio = read(fd, pblk, lrem);
		if (lio == 0) will_exit = 1;
		if (lio != NOSIZE) ldone += lio;
		else xerror(fname);
		if (lio && lio < lrem) {
			pblk += lio;
			lrem -= lio;
			goto _ragain;
		}

		skein_update(&sk, srcblk, ldone);
	}

	close(fd);

	skein_final(dgst, &sk);
	mhexdump(dgst, TF_FROM_BITS(bits), HD_WIDTH);

	return 0;
}
