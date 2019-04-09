#ifndef _TF_PRNG_DEFINITIONS_HEADER
#define _TF_PRNG_DEFINITIONS_HEADER

#include <stdlib.h>
#include "tfdef.h"

#define TF_PRNG_KEY_SIZE TF_KEY_SIZE
#define TF_PRNG_SIZE_UNIT TF_SIZE_UNIT
#define TF_PRNG_RANGE(C, T, S, D) (S + C / ((T)~0 / (D - S + 1) + 1))

size_t tf_prng_datasize(void);
void tf_prng_seedkey_r(void *sdata, const void *skey);
void tf_prng_seedkey(const void *skey);
void tf_prng_genrandom_r(void *sdata, void *result, size_t need);
void tf_prng_genrandom(void *result, size_t need);
void tf_prng_seed_r(void *sdata, TF_UNIT_TYPE seed);
void tf_prng_seed(TF_UNIT_TYPE seed);
TF_UNIT_TYPE tf_prng_random_r(void *sdata);
TF_UNIT_TYPE tf_prng_random(void);
TF_UNIT_TYPE tf_prng_range_r(void *sdata, TF_UNIT_TYPE s, TF_UNIT_TYPE d);
TF_UNIT_TYPE tf_prng_range(TF_UNIT_TYPE s, TF_UNIT_TYPE d);

#endif
