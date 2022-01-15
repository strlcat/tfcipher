#ifndef _TF_STREAM_CIPHER_DEFS
#define _TF_STREAM_CIPHER_DEFS

#include "tfdef.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tfe_stream {
	TF_UNIT_TYPE key[TF_NR_KEY_UNITS];
	TF_UNIT_TYPE iv[TF_NR_BLOCK_UNITS];
	TF_BYTE_TYPE tmp[TF_BLOCK_SIZE];
	size_t tidx;
};

void tfe_init(struct tfe_stream *tfe, const void *key);
void tfe_init_iv(struct tfe_stream *tfe, const void *key, const void *iv);
void tfe_emit(void *dst, size_t szdst, struct tfe_stream *tfe);

#ifdef __cplusplus
}
#endif

#endif
