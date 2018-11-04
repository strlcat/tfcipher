#ifndef _THREEFISH_SKEIN_DEFINITIONS_HEADER
#define _THREEFISH_SKEIN_DEFINITIONS_HEADER

#include "tfdef.h"

#define SKEIN_VERSION 1
#define SKEIN_ID 0x33414853

#define SKEIN_BLOCK_CFG ((TF_UNIT_TYPE)4 << 56)
#define SKEIN_BLOCK_MSG ((TF_UNIT_TYPE)48 << 56)
#define SKEIN_BLOCK_OUT ((TF_UNIT_TYPE)63 << 56)
#define SKEIN_FLAG_FIRST ((TF_UNIT_TYPE)1 << 62)
#define SKEIN_FLAG_LAST ((TF_UNIT_TYPE)1 << 63)

#define SKEIN_DIGEST_SIZE TF_BLOCK_SIZE

struct skein {
	TF_UNIT_TYPE key[TF_NR_KEY_UNITS];
	TF_BYTE_TYPE carry_block[TF_BLOCK_SIZE];
	size_t carry_bytes;
	size_t bits;
};

void skein_init_key(struct skein *sk, const void *ukey, size_t bits);
void skein_init(struct skein *sk, size_t bits);
void skein_update(struct skein *sk, const void *msg, size_t msgsz);
void skein_final(void *result, struct skein *sk);

#endif
