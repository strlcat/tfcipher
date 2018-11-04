#include <string.h>
#include "tfdef.h"

void tf_ecb_encrypt(const void *key, void *out, const void *in, size_t sz)
{
	const TF_BYTE_TYPE *uin = in;
	TF_BYTE_TYPE *uout = out;
	TF_UNIT_TYPE x[TF_NR_BLOCK_UNITS], y[TF_NR_BLOCK_UNITS];
	const TF_UNIT_TYPE *ukey = key;
	size_t sl = sz, i;

	if (sl >= TF_BLOCK_SIZE) {
		do {
			memcpy(x, uin, TF_BLOCK_SIZE);
			uin += TF_BLOCK_SIZE;
			data_to_words(x, TF_BLOCK_SIZE);

			tf_encrypt_rawblk(y, x, ukey);

			data_to_words(y, TF_BLOCK_SIZE);
			memcpy(uout, y, TF_BLOCK_SIZE);
			uout += TF_BLOCK_SIZE;
		} while ((sl -= TF_BLOCK_SIZE) >= TF_BLOCK_SIZE);
	}

	if (sl) {
		memset(x, 0, TF_BLOCK_SIZE);
		memcpy(x, uin, sl);
		data_to_words(x, TF_BLOCK_SIZE);

		memset(y, 0, TF_BLOCK_SIZE);
		tf_encrypt_rawblk(y, y, ukey);
		for (i = 0; i < TF_NR_BLOCK_UNITS; i++) y[i] ^= x[i];

		data_to_words(y, TF_BLOCK_SIZE);
		memcpy(uout, y, sl);
	}

	memset(x, 0, TF_BLOCK_SIZE);
	memset(y, 0, TF_BLOCK_SIZE);
}

void tf_ecb_decrypt(const void *key, void *out, const void *in, size_t sz)
{
	const TF_BYTE_TYPE *uin = in;
	TF_BYTE_TYPE *uout = out;
	TF_UNIT_TYPE x[TF_NR_BLOCK_UNITS], y[TF_NR_BLOCK_UNITS];
	const TF_UNIT_TYPE *ukey = key;
	size_t sl = sz, i;

	if (sl >= TF_BLOCK_SIZE) {
		do {
			memcpy(x, uin, TF_BLOCK_SIZE);
			uin += TF_BLOCK_SIZE;
			data_to_words(x, TF_BLOCK_SIZE);

			tf_decrypt_rawblk(y, x, ukey);

			data_to_words(y, TF_BLOCK_SIZE);
			memcpy(uout, y, TF_BLOCK_SIZE);
			uout += TF_BLOCK_SIZE;
		} while ((sl -= TF_BLOCK_SIZE) >= TF_BLOCK_SIZE);
	}

	if (sl) {
		memset(x, 0, TF_BLOCK_SIZE);
		memcpy(x, uin, sl);
		data_to_words(x, TF_BLOCK_SIZE);

		memset(y, 0, TF_BLOCK_SIZE);
		tf_decrypt_rawblk(y, y, ukey);
		for (i = 0; i < TF_NR_BLOCK_UNITS; i++) y[i] ^= x[i];

		data_to_words(y, TF_BLOCK_SIZE);
		memcpy(uout, y, sl);
	}

	memset(x, 0, TF_BLOCK_SIZE);
	memset(y, 0, TF_BLOCK_SIZE);
}
