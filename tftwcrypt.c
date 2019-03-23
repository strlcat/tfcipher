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

#define SECTSIZE 512

static char key[TF_KEY_SIZE], tweak[TF_TWEAK_SIZE], xtskey[TF_KEY_SIZE], ctr[TF_BLOCK_SIZE];
static char srcblk[DATASIZE], dstblk[DATASIZE];
static struct skein sk;
static int will_exit;

static void usage(void)
{
	printf("usage: tftwcrypt <-e/-d/-we/-wd> key tweak srcfile dstfile.\n");
	printf("-e: encrypt, -d: decrypt.\n");
	printf("-we: encrypt with skein, -wd: decrypt with skein.\n");
	printf("Encrypts srcfile into dstfile with key and tweak using XTS mode.");
	printf("The length of rawkey for -e/-d must be twice the length of cipher key.");
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
	int ifd, ofd;
	int encryptf;
	char *kfname, *twfname, *infname, *onfname;
	size_t lio, lrem, ldone, lblock;
	char *pblk;

	if (argc < 5) usage();
	if (!strcmp(argv[1], "-e")) encryptf = 1;
	else if (!strcmp(argv[1], "-d")) encryptf = 2;
	else if (!strcmp(argv[1], "-we")) encryptf = 3;
	else if (!strcmp(argv[1], "-wd")) encryptf = 4;
	else usage();
	kfname = argv[2];
	twfname = argv[3];
	infname = argv[4];
	onfname = argv[5];
	if (!kfname || !twfname || !infname || !onfname) usage();

	if (!strcmp(kfname, "-")) ifd = 0;
	else {
		ifd = open(kfname, O_RDONLY);
		if (ifd == -1) xerror(kfname);
	}

	if (encryptf < 3) goto _rawkey;
	skein_init(&sk, TF_NR_TWEAKEY_BITS);
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
	skein_init(&sk, TF_NR_TWEAKEY_BITS);
	skein_update(&sk, key, TF_TWEAKEY_SIZE);
	skein_final(xtskey, &sk);
	tf_convkey(key);
	tf_convkey(xtskey);
	goto _keydone;

_rawkey:
	read(ifd, key, TF_TWEAKEY_SIZE);
	tf_convkey(key);
	read(ifd, xtskey, TF_TWEAKEY_SIZE);
	tf_convkey(xtskey);

_keydone:
	if (ifd != 0) close(ifd);

	if (!strcmp(twfname, "-")) ifd = 0;
	else {
		ifd = open(twfname, O_RDONLY);
		if (ifd == -1) xerror(twfname);
	}
	read(ifd, tweak, sizeof(tweak));
	tf_tweak_set(key, tweak);
	tf_tweak_set(xtskey, tweak);
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

		if (encryptf == 1 || encryptf == 3)
			tf_xts_encrypt(key, xtskey, ctr, dstblk, srcblk, ldone, TF_BLOCKS_FROM_BYTES(SECTSIZE));
		else if (encryptf == 2 || encryptf == 4)
			tf_xts_decrypt(key, xtskey, ctr, dstblk, srcblk, ldone, TF_BLOCKS_FROM_BYTES(SECTSIZE));

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

	close(ifd);
	close(ofd);

	return 0;
}
