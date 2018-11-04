#include <string.h>
#include "tfdef.h"
#include "tfcore.h"
#include "skein.h"

static inline void puthash(TF_BYTE_TYPE *dst, const TF_UNIT_TYPE *src, size_t l)
{
	size_t n;
	for (n = 0; n < l; n++) dst[n] = (TF_BYTE_TYPE)(src[n>>3] >> (TF_SIZE_UNIT*(n&7)));
}

static void skein_process_blk(struct skein *sk, const TF_BYTE_TYPE *in, size_t szin, size_t l)
{
	TF_UNIT_TYPE x[TF_NR_BLOCK_UNITS], y[TF_NR_BLOCK_UNITS];
	size_t i;

	do {
		sk->key[TF_TWEAK_WORD1] += l;

		memcpy(x, in, TF_BLOCK_SIZE);
		data_to_words(x, TF_BLOCK_SIZE);
		in += TF_BLOCK_SIZE;

		sk->key[TF_NR_KEY_UNITS-1-3] = THREEFISH_CONST;
		for (i = 0; i < TF_NR_KEY_UNITS-1-3; i++)
			sk->key[TF_NR_KEY_UNITS-1-3] ^= sk->key[i];
		sk->key[TF_TWEAK_WORD3] = sk->key[TF_TWEAK_WORD1] ^ sk->key[TF_TWEAK_WORD2];

		tf_encrypt_rawblk(y, x, sk->key);
		for (i = 0; i < TF_NR_BLOCK_UNITS; i++) sk->key[i] = y[i] ^ x[i];

		sk->key[TF_TWEAK_WORD2] &= ~SKEIN_FLAG_FIRST;
	} while (--szin);
}

void skein_init_key(struct skein *sk, const void *ukey, size_t bits)
{
	TF_UNIT_TYPE cfg[TF_NR_BLOCK_UNITS];

	memset(sk, 0, sizeof(struct skein));

	if (ukey) {
		memcpy(sk->key, ukey, TF_FROM_BITS(TF_MAX_BITS));
		data_to_words(sk->key, TF_FROM_BITS(TF_MAX_BITS));
	}

	sk->bits = bits;
	sk->carry_bytes = 0;

	memset(cfg, 0, sizeof(cfg));
	cfg[0] = TF_SWAP_FUNC(((TF_UNIT_TYPE)SKEIN_VERSION << 32) + (TF_UNIT_TYPE)SKEIN_ID);
	cfg[1] = TF_SWAP_FUNC(bits);

	sk->key[TF_TWEAK_WORD1] = 0;
	sk->key[TF_TWEAK_WORD2] = SKEIN_BLOCK_CFG | SKEIN_FLAG_FIRST | SKEIN_FLAG_LAST;

	skein_process_blk(sk, (TF_BYTE_TYPE *)cfg, 1, 32);

	sk->key[TF_TWEAK_WORD1] = 0;
	sk->key[TF_TWEAK_WORD2] = SKEIN_BLOCK_MSG | SKEIN_FLAG_FIRST;
}

void skein_init(struct skein *sk, size_t bits)
{
	skein_init_key(sk, NULL, bits);
}

void skein_update(struct skein *sk, const void *msg, size_t msgsz)
{
	const TF_BYTE_TYPE *umsg = msg;
	size_t n;

	if (msgsz + sk->carry_bytes > TF_BLOCK_SIZE) {
		if (sk->carry_bytes) {
			n = TF_BLOCK_SIZE - sk->carry_bytes;
			if (n) {
				memcpy(&sk->carry_block[sk->carry_bytes], umsg, n);
				msgsz -= n;
				umsg += n;
				sk->carry_bytes += n;
			}
			skein_process_blk(sk, sk->carry_block, 1, TF_BLOCK_SIZE);
			sk->carry_bytes = 0;
		}

		if (msgsz > TF_BLOCK_SIZE) {
			n = (msgsz-1) / TF_BLOCK_SIZE;
			skein_process_blk(sk, umsg, n, TF_BLOCK_SIZE);
			msgsz -= n * TF_BLOCK_SIZE;
			umsg += n * TF_BLOCK_SIZE;
		}
	}

	if (msgsz) {
		memcpy(&sk->carry_block[sk->carry_bytes], umsg, msgsz);
		sk->carry_bytes += msgsz;
	}
}

void skein_final(void *result, struct skein *sk)
{
	TF_BYTE_TYPE *uresult = result;
	TF_UNIT_TYPE key[TF_NR_BLOCK_UNITS], *X;
	size_t i, b, n;

	if (sk->carry_bytes < TF_BLOCK_SIZE)
		memset(sk->carry_block+sk->carry_bytes, 0, TF_BLOCK_SIZE-sk->carry_bytes);
	sk->key[TF_TWEAK_WORD2] |= SKEIN_FLAG_LAST;
	skein_process_blk(sk, sk->carry_block, 1, sk->carry_bytes);

	b = (sk->bits + 7) / 8;

	memset(sk->carry_block, 0, sizeof(sk->carry_block));
	memcpy(key, sk->key, sizeof(key));

	for (i = 0; (i * TF_BLOCK_SIZE) < b; i++) {
		X = (TF_UNIT_TYPE *)sk->carry_block;
		X[0] = TF_SWAP_FUNC((TF_UNIT_TYPE)i);
		sk->key[TF_TWEAK_WORD1] = 0;
		sk->key[TF_TWEAK_WORD2] = SKEIN_BLOCK_OUT | SKEIN_FLAG_FIRST | SKEIN_FLAG_LAST;
		sk->carry_bytes = 0;

		skein_process_blk(sk, sk->carry_block, 1, TF_SIZE_UNIT);
		n = b-(i*TF_BLOCK_SIZE);
		if (n >= TF_BLOCK_SIZE) n = TF_BLOCK_SIZE;
		puthash(uresult+(i*TF_BLOCK_SIZE), sk->key, n);
		memcpy(sk->key, key, TF_BLOCK_SIZE);
	}

	memset(sk, 0, sizeof(struct skein));
}
