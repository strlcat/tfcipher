#ifndef _TF_STREAM_CIPHER_DEFS
#define _TF_STREAM_CIPHER_DEFS

#include "tfdef.h"

struct tfe_stream {
	TF_UNIT_TYPE key[TF_NR_KEY_UNITS];
	TF_UNIT_TYPE iv[TF_NR_BLOCK_UNITS];
	TF_BYTE_TYPE carry_block[TF_BLOCK_SIZE];
	size_t carry_bytes;
};

void tfe_init(struct tfe_stream *tfe, const void *key);
void tfe_init_iv(struct tfe_stream *tfe, const void *key, const void *iv);
void tfe_emit(void *dst, size_t szdst, struct tfe_stream *tfe);

#endif
