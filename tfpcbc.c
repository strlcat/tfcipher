#include <string.h>
#include "tfdef.h"

void tf_pcbc_encrypt(const void *key, void *iv, void *out, const void *in, size_t sz)
{
	const TF_BYTE_TYPE *uin = (const TF_BYTE_TYPE *)in;
	TF_BYTE_TYPE *uout = (TF_BYTE_TYPE *)out;
	TF_UNIT_TYPE x[TF_NR_BLOCK_UNITS], y[TF_NR_BLOCK_UNITS];
	TF_UNIT_TYPE *uiv = (TF_UNIT_TYPE *)iv;
	const TF_UNIT_TYPE *ukey = (const TF_UNIT_TYPE *)key;
	size_t sl = sz, i;

	if (sl >= TF_BLOCK_SIZE) {
		do {
			memcpy(x, uin, TF_BLOCK_SIZE);
			uin += TF_BLOCK_SIZE;
			data_to_words(x, TF_BLOCK_SIZE);

			for (i = 0; i < TF_NR_BLOCK_UNITS; i++) y[i] = x[i] ^ uiv[i];
			tf_encrypt_rawblk(y, y, ukey);
			for (i = 0; i < TF_NR_BLOCK_UNITS; i++) uiv[i] = y[i] ^ x[i];

			data_to_words(y, TF_BLOCK_SIZE);
			memcpy(uout, y, TF_BLOCK_SIZE);
			uout += TF_BLOCK_SIZE;
		} while ((sl -= TF_BLOCK_SIZE) >= TF_BLOCK_SIZE);
	}

	if (sl) {
		memset(x, 0, TF_BLOCK_SIZE);
		memcpy(x, uin, sl);
		data_to_words(x, TF_BLOCK_SIZE);

		ctr_inc(uiv, TF_NR_BLOCK_UNITS);
		tf_encrypt_rawblk(y, uiv, ukey);
		for (i = 0; i < TF_NR_BLOCK_UNITS; i++) y[i] ^= x[i];

		data_to_words(y, TF_BLOCK_SIZE);
		memcpy(uout, y, sl);
	}

	memset(x, 0, TF_BLOCK_SIZE);
	memset(y, 0, TF_BLOCK_SIZE);
}

void tf_pcbc_decrypt(const void *key, void *iv, void *out, const void *in, size_t sz)
{
	const TF_BYTE_TYPE *uin = (const TF_BYTE_TYPE *)in;
	TF_BYTE_TYPE *uout = (TF_BYTE_TYPE *)out;
	TF_UNIT_TYPE x[TF_NR_BLOCK_UNITS], y[TF_NR_BLOCK_UNITS];
	TF_UNIT_TYPE *uiv = (TF_UNIT_TYPE *)iv;
	const TF_UNIT_TYPE *ukey = (const TF_UNIT_TYPE *)key;
	size_t sl = sz, i;

	if (sl >= TF_BLOCK_SIZE) {
		do {
			memcpy(x, uin, TF_BLOCK_SIZE);
			uin += TF_BLOCK_SIZE;
			data_to_words(x, TF_BLOCK_SIZE);

			tf_decrypt_rawblk(y, x, ukey);
			for (i = 0; i < TF_NR_BLOCK_UNITS; i++) y[i] ^= uiv[i];
			for (i = 0; i < TF_NR_BLOCK_UNITS; i++) uiv[i] = y[i] ^ x[i];

			data_to_words(y, TF_BLOCK_SIZE);
			memcpy(uout, y, TF_BLOCK_SIZE);
			uout += TF_BLOCK_SIZE;
		} while ((sl -= TF_BLOCK_SIZE) >= TF_BLOCK_SIZE);
	}

	if (sl) {
		memset(x, 0, TF_BLOCK_SIZE);
		memcpy(x, uin, sl);
		data_to_words(x, TF_BLOCK_SIZE);

		ctr_inc(uiv, TF_NR_BLOCK_UNITS);
		tf_encrypt_rawblk(y, uiv, ukey);
		for (i = 0; i < TF_NR_BLOCK_UNITS; i++) y[i] ^= x[i];

		data_to_words(y, TF_BLOCK_SIZE);
		memcpy(uout, y, sl);
	}

	memset(x, 0, TF_BLOCK_SIZE);
	memset(y, 0, TF_BLOCK_SIZE);
}
