#include <string.h>
#include "tfdef.h"
#include "tfcore.h"

void tf_tweak_set(void *key, const void *tweak)
{
	TF_UNIT_TYPE *ukey = (TF_UNIT_TYPE *)key;
	TF_UNIT_TYPE *twe = ukey+TF_TWEAK_WORD1;
	TF_UNIT_TYPE c = THREEFISH_CONST;
	size_t x;

	for (x = 0; x < TF_NR_BLOCK_UNITS; x++) c ^= ukey[x];
	ukey[x] = c;

	if (!tweak) {
		memset(twe, 0, (TF_NR_TWEAK_UNITS+1)*TF_SIZE_UNIT);
		return;
	}

	memcpy(twe, tweak, TF_NR_TWEAK_UNITS*TF_SIZE_UNIT);
	data_to_words(twe, TF_NR_TWEAK_UNITS*TF_SIZE_UNIT);
	ukey[TF_TWEAK_WORD3] = ukey[TF_TWEAK_WORD1] ^ ukey[TF_TWEAK_WORD2];
}

void tf_tweak_inc(void *key)
{
	TF_UNIT_TYPE *twe = (TF_UNIT_TYPE *)key+TF_TWEAK_WORD1;
	ctr_inc(twe, TF_NR_TWEAK_UNITS);
}
