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
#include "tfe.h"
#include "tfsupport.h"

static char key[TF_KEY_SIZE];
static char srcblk[DATASIZE], dstblk[DATASIZE];
static struct skein sk;
static struct tfe_stream tfe;
static int will_exit;

static void usage(void)
{
	printf("usage: tfecrypt key srcfile dstfile.\n");
	printf("This program is part of Threefish stream cipher reference.\n");
	exit(1);
}

static void xerror(const char *s)
{
	perror(s);
	exit(2);
}

int main(int argc, char **argv)
{
	int ifd, ofd;
	char *kfname, *infname, *onfname;
	size_t lio, lrem, ldone, lblock;
	char *pblk;

	if (argc < 4) usage();
	kfname = argv[1];
	infname = argv[2];
	onfname = argv[3];
	if (!kfname || !infname || !onfname) usage();

	if (!strcmp(kfname, "-")) ifd = 0;
	else {
		ifd = open(kfname, O_RDONLY);
		if (ifd == -1) xerror(kfname);
	}

	skein_init(&sk, TF_NR_KEY_BITS);
	will_exit = 0;
	while (1) {
		if (will_exit) break;
		pblk = srcblk;
		ldone = 0;
		lrem = lblock = sizeof(srcblk);
_rkagain:	lio = read(ifd, pblk, lrem);
		if (lio == 0) will_exit = 1;
		if (lio != NOSIZE) ldone += lio;
		else xerror(kfname);
		if (lio && lio < lrem) {
			pblk += lio;
			lrem -= lio;
			goto _rkagain;
		}
		skein_update(&sk, srcblk, ldone);
	}
	skein_final(key, &sk);

	if (ifd != 0) close(ifd);

	if (!strcmp(infname, "-")) ifd = 0;
	else {
		ifd = open(infname, O_RDONLY);
		if (ifd == -1) xerror(infname);
	}

	if (!strcmp(onfname, "-")) ofd = 1;
	else {
		ofd = creat(onfname, 0666);
		if (ofd == -1) xerror(onfname);
	}

	tfe_init(&tfe, key);
	will_exit = 0;
	while (1) {
		if (will_exit) break;
		pblk = srcblk;
		ldone = 0;
		lrem = lblock = sizeof(srcblk);
_ragain:	lio = read(ifd, pblk, lrem);
		if (lio == 0) will_exit = 1;
		if (lio != NOSIZE) ldone += lio;
		else xerror(infname);
		if (lio && lio < lrem) {
			pblk += lio;
			lrem -= lio;
			goto _ragain;
		}

		tf_stream_crypt(&tfe, dstblk, srcblk, ldone);

		pblk = dstblk;
		lrem = ldone;
		ldone = 0;
_wagain:	lio = write(ofd, pblk, lrem);
		if (lio != NOSIZE) ldone += lio;
		else xerror(onfname);
		if (lio < lrem) {
			pblk += lio;
			lrem -= lio;
			goto _wagain;
		}
	}
	tfe_emit(NULL, 0, &tfe);

	close(ifd);
	close(ofd);

	return 0;
}
