#include <string.h>
#include "tfdef.h"
#include "tfsupport.h"

static char data[TF_BLOCK_SIZE] = "Lynx Rys";
static char key[TF_KEY_SIZE] = "\a\x76syR_\x98?";
static char dkey[TF_KEY_SIZE] = "\a\x77syR_\x98?";

int main(int argc, char **argv)
{
	if (argc >= 2) xzstrlcpy(key, argv[1], sizeof(key));
	if (argc >= 3) xzstrlcpy(data, argv[2], sizeof(data));
	if (argc >= 4) xzstrlcpy(dkey, argv[3], sizeof(dkey));

	tf_convkey(key);
	mhexdump(data, TF_BLOCK_SIZE, HD_WIDTH);
	newline();
	tf_encrypt_block(key, data, data);
	mhexdump(data, TF_BLOCK_SIZE, HD_WIDTH);
	newline();
	tf_decrypt_block(key, data, data);
	mhexdump(data, TF_BLOCK_SIZE, HD_WIDTH);
	newline();

	tf_encrypt_block(key, data, data);
	tf_convkey(dkey);
	tf_decrypt_block(dkey, data, data);
	mhexdump(data, TF_BLOCK_SIZE, HD_WIDTH);

	return 0;
}
