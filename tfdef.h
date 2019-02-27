#ifndef _THREEFISH_CIPHER_DEFINITIONS_HEADER
#define _THREEFISH_CIPHER_DEFINITIONS_HEADER

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif

/* config block */
#define TF_256BITS
/* #define TF_512BITS */
/* #define TF_1024BITS */
/* #define TF_NO_ENDIAN */
/* #define TF_BIG_ENDIAN */

#include <stddef.h>
#include <stdint.h>
#ifndef TF_NO_ENDIAN
#include <sys/param.h>
#else
#undef TF_BIG_ENDIAN
#endif

#define TF_UNIT_TYPE uint64_t

#ifdef TF_BIG_ENDIAN
#define TF_SWAP_FUNC htobe64
#else
#define TF_SWAP_FUNC htole64
#endif

#if defined(TF_256BITS)
#define TF_NR_BLOCK_BITS 256
#define TF_NR_KEY_BITS 512
#define TF_NR_BLOCK_UNITS 4
#define TF_NR_KEY_UNITS 8
#define IRR_POLY_CONST 0x425
#elif defined(TF_512BITS)
#define TF_NR_BLOCK_BITS 512
#define TF_NR_KEY_BITS 768
#define TF_NR_BLOCK_UNITS 8
#define TF_NR_KEY_UNITS 12
#define IRR_POLY_CONST 0x125
#elif defined(TF_1024BITS)
#define TF_NR_BLOCK_BITS 1024
#define TF_NR_KEY_BITS 1280
#define TF_NR_BLOCK_UNITS 16
#define TF_NR_KEY_UNITS 20
#define IRR_POLY_CONST 0x80043
#else
#error Please edit tfdef.h include file and select at least one cipher!
#endif

#define TF_BYTE_TYPE uint8_t
#define TF_SIZE_UNIT (sizeof(TF_UNIT_TYPE))
#define TF_BLOCK_SIZE (TF_SIZE_UNIT * TF_NR_BLOCK_UNITS)
#define TF_KEY_SIZE (TF_SIZE_UNIT * TF_NR_KEY_UNITS)

#define TF_TWEAK_WORD1 (TF_NR_KEY_UNITS-3)
#define TF_TWEAK_WORD2 (TF_NR_KEY_UNITS-2)
#define TF_TWEAK_WORD3 (TF_NR_KEY_UNITS-1)

#define TF_TO_BITS(x) ((x) * 8)
#define TF_FROM_BITS(x) ((x) / 8)
#define TF_MAX_BITS TF_NR_BLOCK_BITS
#define TF_UNIT_BITS (TF_SIZE_UNIT * 8)

#define TF_TO_BLOCKS(x) ((x) / TF_BLOCK_SIZE)
#define TF_FROM_BLOCKS(x) ((x) * TF_BLOCK_SIZE)
#define TF_BLOCKS_TO_BYTES(x) TF_FROM_BLOCKS(x)
#define TF_BLOCKS_FROM_BYTES(x) TF_TO_BLOCKS(x)

static inline void data_to_words(void *p, size_t l)
{
#ifndef TF_NO_ENDIAN
	size_t idx;
	TF_UNIT_TYPE *P = p;
	TF_UNIT_TYPE t;

	for (idx = 0; idx < (l/sizeof(TF_UNIT_TYPE)); idx++) {
		t = TF_SWAP_FUNC(P[idx]);
		P[idx] = t;
	}
#endif
}

static inline void ctr_inc(TF_UNIT_TYPE *x, size_t xl)
{
	size_t z;

	for (z = 0; z < xl; z++) {
		x[z] = ((x[z] + (TF_UNIT_TYPE)1) & ((TF_UNIT_TYPE)~0));
		if (x[z]) break;
	}
}

static inline void ctr_add(TF_UNIT_TYPE *x, size_t xl, const TF_UNIT_TYPE *y, size_t yl)
{
	size_t z, cf;
	TF_UNIT_TYPE t;

	for (z = 0, cf = 0; z < xl; z++) {
		t = x[z] + (z >= yl ? (TF_UNIT_TYPE)0 : y[z]) + cf;
		if (cf) cf = (x[z] >= t ? 1 : 0);
		else cf = (x[z] > t ? 1 : 0);
		x[z] = t;
	}
}

struct tfe_stream;

#define tf_convkey(k) do { data_to_words(k, TF_KEY_SIZE); } while (0)

void tf_encrypt_rawblk(TF_UNIT_TYPE *O, const TF_UNIT_TYPE *I, const TF_UNIT_TYPE *K);
void tf_decrypt_rawblk(TF_UNIT_TYPE *O, const TF_UNIT_TYPE *I, const TF_UNIT_TYPE *K);

void tf_encrypt_block(const void *key, void *out, const void *in);
void tf_decrypt_block(const void *key, void *out, const void *in);

void tf_ctr_set(void *ctr, const void *sctr, size_t sctrsz);
void tf_ctr_crypt(const void *key, void *ctr, void *out, const void *in, size_t sz);
void tf_stream_crypt(struct tfe_stream *tfe, void *out, const void *in, size_t sz);
void tf_ecb_encrypt(const void *key, void *out, const void *in, size_t sz);
void tf_ecb_decrypt(const void *key, void *out, const void *in, size_t sz);
void tf_cbc_encrypt(const void *key, void *iv, void *out, const void *in, size_t sz);
void tf_cbc_decrypt(const void *key, void *iv, void *out, const void *in, size_t sz);
void tf_pcbc_encrypt(const void *key, void *iv, void *out, const void *in, size_t sz);
void tf_pcbc_decrypt(const void *key, void *iv, void *out, const void *in, size_t sz);
void tf_xts_encrypt(const void *keyx, const void *keyz, void *ctr, void *out, const void *in, size_t sz, size_t bpi);
void tf_xts_decrypt(const void *keyx, const void *keyz, void *ctr, void *out, const void *in, size_t sz, size_t bpi);
void tf_ocb_encrypt(const void *key, void *ctr, void *out, void *tag, const void *in, size_t sz, size_t bpi);
void tf_ocb_decrypt(const void *key, void *ctr, void *out, void *tag, const void *in, size_t sz, size_t bpi);

#endif
