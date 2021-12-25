#include <string.h>
#include "tfdef.h"

void tf_ctr_set(void *ctr, const void *sctr, size_t sctrsz)
{
	TF_UNIT_TYPE usctr[TF_NR_BLOCK_UNITS];
	TF_UNIT_TYPE *uctr = (TF_UNIT_TYPE *)ctr;

	data_to_words(uctr, TF_BLOCK_SIZE);

	memset(usctr, 0, TF_BLOCK_SIZE);
	memcpy(usctr, sctr, sctrsz > TF_BLOCK_SIZE ? TF_BLOCK_SIZE : sctrsz);
	data_to_words(usctr, TF_BLOCK_SIZE);

	ctr_add(uctr, TF_NR_BLOCK_UNITS, usctr, TF_NR_BLOCK_UNITS);
	memset(usctr, 0, TF_BLOCK_SIZE);
}
