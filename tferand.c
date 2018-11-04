#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "tfdef.h"
#include "tfe.h"
#include "tfsupport.h"

static char data[DATASIZE];
static char key[TF_KEY_SIZE];
static tf_fsize howmuch = NOFSIZE, generated;
static struct tfe_stream tfe;

int main(int argc, char **argv)
{
	int fd;
	char *stoi;
	size_t x;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd != -1) {
		read(fd, key, sizeof(key));
		close(fd);
	}

	if (argc >= 2) {
		howmuch = tf_humanfsize(argv[1], &stoi);
		if (*stoi) howmuch = NOFSIZE;
	}

	tfe_init(&tfe, key);
	while (1) {
		x = blk_len_adjust(howmuch, generated, sizeof(data));
		tfe_emit(data, x, &tfe);
		if (write(1, data, x) == -1) return 1;
		generated += x;
		if (howmuch != NOFSIZE && generated >= howmuch) break;
	}
	tfe_emit(NULL, 0, &tfe);

	return 0;
}
