#include <string.h>
#include "tfdef.h"

void tf_encrypt_block(const void *key, void *out, const void *in)
{
	TF_UNIT_TYPE x[TF_NR_BLOCK_UNITS];
	const TF_UNIT_TYPE *ukey = (TF_UNIT_TYPE *)key;
	TF_UNIT_TYPE *uout = (TF_UNIT_TYPE *)out;

	memcpy(x, in, TF_BLOCK_SIZE);
	data_to_words(x, TF_BLOCK_SIZE);
	tf_encrypt_rawblk(uout, x, ukey);
	data_to_words(uout, TF_BLOCK_SIZE);

	memset(x, 0, TF_BLOCK_SIZE);
}

void tf_decrypt_block(const void *key, void *out, const void *in)
{
	TF_UNIT_TYPE x[TF_NR_BLOCK_UNITS];
	const TF_UNIT_TYPE *ukey = (TF_UNIT_TYPE *)key;
	TF_UNIT_TYPE *uout = (TF_UNIT_TYPE *)out;

	memcpy(x, in, TF_BLOCK_SIZE);
	data_to_words(x, TF_BLOCK_SIZE);
	tf_decrypt_rawblk(uout, x, ukey);
	data_to_words(uout, TF_BLOCK_SIZE);

	memset(x, 0, TF_BLOCK_SIZE);
}
