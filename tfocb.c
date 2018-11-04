#include <string.h>
#include "tfdef.h"

static inline void ocb_block(TF_UNIT_TYPE *x, int tag)
{
	TF_UNIT_TYPE c = (x[0] >> (TF_UNIT_BITS-1));
	size_t i;

	if (tag) goto _tag;
	for (i = 0; i < TF_NR_BLOCK_UNITS-1; i++)
		x[i] = ((x[i] << 1) | (x[i+1] >> (TF_UNIT_BITS-1)));
	x[TF_NR_BLOCK_UNITS-1] = ((x[i-1] << 1) ^ (c*IRR_POLY_CONST));
	return;

_tag:	for (i = 0; i < TF_NR_BLOCK_UNITS-1; i++)
		x[i] ^= ((x[i] << 1) | (x[i+1] >> (TF_UNIT_BITS-1)));
	x[TF_NR_BLOCK_UNITS-1] ^= ((x[i-1] << 1) ^ (c*IRR_POLY_CONST));
}

static void ocb_encrypt(const void *key, void *ctr, void *out, void *tag, const void *in, size_t sz)
{
	const TF_BYTE_TYPE *uin = in;
	TF_BYTE_TYPE *uout = out, *s, *d;
	TF_UNIT_TYPE x[TF_NR_BLOCK_UNITS], y[TF_NR_BLOCK_UNITS];
	TF_UNIT_TYPE tctr[TF_NR_BLOCK_UNITS], c[TF_NR_BLOCK_UNITS];
	TF_UNIT_TYPE *uctr = ctr, *utag = tag;
	const TF_UNIT_TYPE *ukey = key;
	size_t sl = sz, i;

	tf_encrypt_rawblk(tctr, uctr, ukey);
	if (tag) {
		memcpy(c, tag, TF_BLOCK_SIZE);
		data_to_words(c, TF_BLOCK_SIZE);
	}

	if (sl >= TF_BLOCK_SIZE) {
		do {
			memcpy(x, uin, TF_BLOCK_SIZE);
			uin += TF_BLOCK_SIZE;
			data_to_words(x, TF_BLOCK_SIZE);

			ctr_inc(uctr, TF_NR_BLOCK_UNITS);
			ocb_block(tctr, 0);
			if (tag) for (i = 0; i < TF_NR_BLOCK_UNITS; i++) c[i] ^= x[i];
			for (i = 0; i < TF_NR_BLOCK_UNITS; i++) x[i] ^= tctr[i];
			tf_encrypt_rawblk(y, x, ukey);
			for (i = 0; i < TF_NR_BLOCK_UNITS; i++) y[i] ^= tctr[i];

			data_to_words(y, TF_BLOCK_SIZE);
			memcpy(uout, y, TF_BLOCK_SIZE);
			uout += TF_BLOCK_SIZE;
		} while ((sl -= TF_BLOCK_SIZE) >= TF_BLOCK_SIZE);
	}

	if (sl) {
		ctr_inc(uctr, TF_NR_BLOCK_UNITS);
		ocb_block(tctr, 0);
		memset(x, 0, TF_BLOCK_SIZE);
		x[TF_NR_BLOCK_UNITS-1] = (TF_TO_BITS(sl));
		for (i = 0; i < TF_NR_BLOCK_UNITS; i++) x[i] ^= tctr[i];
		tf_encrypt_rawblk(y, x, ukey);

		memcpy(x, uin, sl);
		data_to_words(x, sl);
		s = (TF_BYTE_TYPE *)x; d = (TF_BYTE_TYPE *)y;
		memcpy(s+sl, d+sl, TF_BLOCK_SIZE-sl);
		if (tag) for (i = 0; i < TF_NR_BLOCK_UNITS; i++) c[i] ^= x[i];
		for (i = 0; i < TF_NR_BLOCK_UNITS; i++) x[i] ^= y[i];

		data_to_words(x, sl);
		memcpy(uout, x, sl);
	}

	if (!tag) goto _done;

	ocb_block(tctr, 1);
	for (i = 0; i < TF_NR_BLOCK_UNITS; i++) x[i] = tctr[i] ^ c[i];
	tf_encrypt_rawblk(y, x, ukey);
	data_to_words(y, TF_BLOCK_SIZE);
	for (i = 0; i < TF_NR_BLOCK_UNITS; i++) utag[i] ^= y[i];

_done:	memset(tctr, 0, TF_BLOCK_SIZE);
	memset(c, 0, TF_BLOCK_SIZE);
	memset(x, 0, TF_BLOCK_SIZE);
	memset(y, 0, TF_BLOCK_SIZE);
}

void tf_ocb_encrypt(const void *key, void *ctr, void *out, void *tag, const void *in, size_t sz, size_t bpi)
{
	const TF_BYTE_TYPE *uin = in;
	TF_BYTE_TYPE *uout = out;
	size_t sl = sz, sx = TF_BLOCKS_TO_BYTES(bpi);

	if (sl >= sx) {
		do {
			ocb_encrypt(key, ctr, uout, tag, uin, sx);
			uout += sx;
			uin += sx;
		} while ((sl -= sx) >= sx);
	}

	if (sl) ocb_encrypt(key, ctr, uout, tag, uin, sl);
}

static void ocb_decrypt(const void *key, void *ctr, void *out, void *tag, const void *in, size_t sz)
{
	const TF_BYTE_TYPE *uin = in;
	TF_BYTE_TYPE *uout = out;
	TF_UNIT_TYPE x[TF_NR_BLOCK_UNITS], y[TF_NR_BLOCK_UNITS];
	TF_UNIT_TYPE tctr[TF_NR_BLOCK_UNITS], c[TF_NR_BLOCK_UNITS];
	TF_UNIT_TYPE *uctr = ctr, *utag = tag;
	const TF_UNIT_TYPE *ukey = key;
	size_t sl = sz, i;

	tf_encrypt_rawblk(tctr, uctr, ukey);
	if (tag) {
		memcpy(c, tag, TF_BLOCK_SIZE);
		data_to_words(c, TF_BLOCK_SIZE);
	}

	if (sl >= TF_BLOCK_SIZE) {
		do {
			memcpy(x, uin, TF_BLOCK_SIZE);
			uin += TF_BLOCK_SIZE;
			data_to_words(x, TF_BLOCK_SIZE);

			ctr_inc(uctr, TF_NR_BLOCK_UNITS);
			ocb_block(tctr, 0);
			for (i = 0; i < TF_NR_BLOCK_UNITS; i++) x[i] ^= tctr[i];
			tf_decrypt_rawblk(y, x, ukey);
			for (i = 0; i < TF_NR_BLOCK_UNITS; i++) y[i] ^= tctr[i];
			if (tag) for (i = 0; i < TF_NR_BLOCK_UNITS; i++) c[i] ^= y[i];

			data_to_words(y, TF_BLOCK_SIZE);
			memcpy(uout, y, TF_BLOCK_SIZE);
			uout += TF_BLOCK_SIZE;
		} while ((sl -= TF_BLOCK_SIZE) >= TF_BLOCK_SIZE);
	}

	if (sl) {
		ctr_inc(uctr, TF_NR_BLOCK_UNITS);
		ocb_block(tctr, 0);
		memset(x, 0, TF_BLOCK_SIZE);
		x[TF_NR_BLOCK_UNITS-1] = (TF_TO_BITS(sl));
		for (i = 0; i < TF_NR_BLOCK_UNITS; i++) x[i] ^= tctr[i];
		tf_encrypt_rawblk(y, x, ukey);

		memset(x, 0, TF_BLOCK_SIZE);
		memcpy(x, uin, sl);
		data_to_words(x, sl);
		for (i = 0; i < TF_NR_BLOCK_UNITS; i++) x[i] ^= y[i];
		if (tag) for (i = 0; i < TF_NR_BLOCK_UNITS; i++) c[i] ^= x[i];

		data_to_words(x, sl);
		memcpy(uout, x, sl);
	}

	if (!tag) goto _done;

	ocb_block(tctr, 1);
	for (i = 0; i < TF_NR_BLOCK_UNITS; i++) x[i] = tctr[i] ^ c[i];
	tf_encrypt_rawblk(y, x, ukey);
	data_to_words(y, TF_BLOCK_SIZE);
	for (i = 0; i < TF_NR_BLOCK_UNITS; i++) utag[i] ^= y[i];

_done:	memset(tctr, 0, TF_BLOCK_SIZE);
	memset(c, 0, TF_BLOCK_SIZE);
	memset(x, 0, TF_BLOCK_SIZE);
	memset(y, 0, TF_BLOCK_SIZE);
}

void tf_ocb_decrypt(const void *key, void *ctr, void *out, void *tag, const void *in, size_t sz, size_t bpi)
{
	const TF_BYTE_TYPE *uin = in;
	TF_BYTE_TYPE *uout = out;
	size_t sl = sz, sx = TF_BLOCKS_TO_BYTES(bpi);

	if (sl >= sx) {
		do {
			ocb_decrypt(key, ctr, uout, tag, uin, sx);
			uout += sx;
			uin += sx;
		} while ((sl -= sx) >= sx);
	}

	if (sl) ocb_decrypt(key, ctr, uout, tag, uin, sl);
}
