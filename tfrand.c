#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "tfdef.h"
#include "tfsupport.h"

static char data[DATASIZE];
static char key[TF_KEY_SIZE], ctr[TF_BLOCK_SIZE];
static tf_fsize howmuch = NOFSIZE, generated;

int main(int argc, char **argv)
{
	int fd;
	char *stoi;
	size_t x;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd != -1) {
		read(fd, key, sizeof(key));
		read(fd, ctr, sizeof(ctr));
		close(fd);
	}
	tf_convkey(key);

	if (argc >= 2) {
		howmuch = tf_humanfsize(argv[1], &stoi);
		if (*stoi) howmuch = NOFSIZE;
	}

	while (1) {
		x = blk_len_adjust(howmuch, generated, sizeof(data));
		tf_ctr_crypt(key, ctr, data, data, x);
		if (write(1, data, x) == -1) return 1;
		generated += x;
		if (howmuch != NOFSIZE && generated >= howmuch) break;
	}

	return 0;
}
