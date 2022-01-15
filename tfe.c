#include <string.h>
#include "tfdef.h"
#include "tfe.h"

void tfe_init_iv(struct tfe_stream *tfe, const void *key, const void *iv)
{
	memset(tfe, 0, sizeof(struct tfe_stream));
	memcpy(tfe->key, key, TF_KEY_SIZE);
	if (iv) memcpy(tfe->iv, iv, TF_BLOCK_SIZE);
	tfe->tidx = 0;
}

void tfe_init(struct tfe_stream *tfe, const void *key)
{
	tfe_init_iv(tfe, key, NULL);
}

void tfe_emit(void *dst, size_t szdst, struct tfe_stream *tfe)
{
	TF_BYTE_TYPE *udst = dst;
	size_t sz = szdst, trem;

	if (!dst && szdst == 0) {
		memset(tfe, 0, sizeof(struct tfe_stream));
		return;
	}

	if (tfe->tidx > 0) {
		trem = TF_BLOCK_SIZE-tfe->tidx;

		if (szdst <= trem) {
			memcpy(udst, &tfe->tmp[tfe->tidx], szdst);
			tfe->tidx += szdst;
			if (tfe->tidx >= TF_BLOCK_SIZE) tfe->tidx = 0;
			return;
		}

		memcpy(udst, &tfe->tmp[tfe->tidx], trem);
		udst += trem;
		sz -= trem;
		tfe->tidx = 0;
	}

	if (sz >= TF_BLOCK_SIZE) {
		do {
			tf_encrypt_rawblk(tfe->iv, tfe->iv, tfe->key);
			memcpy(udst, tfe->iv, TF_BLOCK_SIZE);
			data_to_words(udst, TF_BLOCK_SIZE);
			udst += TF_BLOCK_SIZE;
		} while ((sz -= TF_BLOCK_SIZE) >= TF_BLOCK_SIZE);
	}

	if (sz) {
		tf_encrypt_rawblk(tfe->iv, tfe->iv, tfe->key);
		memcpy(tfe->tmp, tfe->iv, TF_BLOCK_SIZE);
		data_to_words(tfe->tmp, TF_BLOCK_SIZE);
		memcpy(udst, tfe->tmp, sz);
		tfe->tidx = sz;
	}
}
