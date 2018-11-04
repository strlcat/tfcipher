#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "tfdef.h"
#include "tfe.h"
#include "tfsupport.h"

static uint64_t encbytes;
static char key[TF_KEY_SIZE] = "\a\x76syR_\x98?";
static char data[DATASIZE];
static struct tfe_stream tfe;
static unsigned do_break;

static void do_stop_sig(int unused)
{
	do_break = 1;
}

int main(void)
{
	tfe_init(&tfe, key);

	printf("Doing stream encryption loop for next %u seconds...\n", TESTTIME);
	signal(SIGALRM, do_stop_sig);
	alarm(TESTTIME);

	while (1) {
		if (do_break) break;
		tfe_emit(data, sizeof(data), &tfe);
		encbytes += sizeof(data);
	}
	tfe_emit(NULL, 0, &tfe);

	printf("Done. Stats: encrypted: %llu, byps: %llu\n", encbytes, encbytes / TESTTIME);

	puts("Threefish stream cipher testing program done.");

	return 0;
}
