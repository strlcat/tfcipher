#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "tfdef.h"
#include "tfsupport.h"

static uint64_t encbytes, decbytes;
static char sdata[DATASIZE], udata[DATASIZE];
static char key[TF_KEY_SIZE] = "\a\x76syR_\x98?";
static unsigned do_break;

static void do_stop_sig(int unused)
{
	do_break = 1;
}

int main(void)
{
	memset(sdata, 'X', sizeof(sdata));

	tf_convkey(key);

	printf("Doing encryption loop for next %u seconds...\n", TESTTIME);
	signal(SIGALRM, do_stop_sig);
	alarm(TESTTIME);

	tf_ecb_encrypt(key, udata, sdata, sizeof(sdata));
	while (1) {
		if (do_break) break;
		tf_ecb_encrypt(key, udata, udata, sizeof(udata));
		encbytes += sizeof(udata);
	}

	puts("Encrypted ECB blocks:");
	mhexdump(udata, TF_BLOCK_SIZE * 2, HD_WIDTH);
	puts("...");

	puts("Done. Doing decryption of encrypted cell...");
	while (1) {
		tf_ecb_decrypt(key, udata, udata, sizeof(udata));
		decbytes += sizeof(udata);
		if (decbytes >= encbytes) break;
	}
	tf_ecb_decrypt(key, udata, udata, sizeof(udata));

	if (!memcmp(udata, sdata, sizeof(sdata)))
		puts("\n*** Cells are identical, cipher succeeded! ***\n");

	printf("Stats: encrypted: %llu, byps: %llu\n", encbytes, decbytes / TESTTIME);

	puts("Threefish cipher testing program done.");

	return 0;
}
