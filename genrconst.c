#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "tfdef.h"
#include "tfprng.h"
#include "tfsupport.h"

#define ROT_FROM 4
#define ROT_TO (TF_UNIT_BITS-ROT_FROM-1)

#if defined(TF_256BITS)
#define KS_ROTS 4
#define BS_ROTS 12
#elif defined(TF_512BITS)
#define KS_ROTS 8
#define BS_ROTS 24
#elif defined(TF_1024BITS)
#define KS_ROTS 16
#define BS_ROTS 48
#endif

static char key[TF_KEY_SIZE];

int main(void)
{
	size_t x, t;
	int fd;

	fd = open("/dev/random", O_RDONLY);
	if (fd != -1) {
		if (read(fd, key, sizeof(key)) != sizeof(key)) return 2;
		close(fd);
	}
	else return 1;

	tf_prng_seedkey(key);

	printf("enum tf_rotations {\n");
	for (x = 0; x < KS_ROTS; x++) {
		t = tf_prng_range(ROT_FROM, ROT_TO);
		if (((x)%4) == 0) printf("\t");
		printf(((x+1)%4) ? "TFS_KS%02zu = % 2zu, " : "TFS_KS%02zu = % 2zu,\n", x+1, t);
	}
	for (x = 0; x < BS_ROTS; x++) {
		t = tf_prng_range(ROT_FROM, ROT_TO);
		if (((x)%4) == 0) printf("\t");
		printf(((x+1)%4) ? "TFS_BS%02zu = % 2zu, " : "TFS_BS%02zu = % 2zu,\n", x+1, t);
	}
	printf("};\n");

	tf_prng_seedkey(NULL);

	return 0;
}
