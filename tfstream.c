#include <string.h>
#include "tfdef.h"
#include "tfe.h"

static inline void xor_block(void *dst, const void *src, size_t sz)
{
	const size_t *sx = (const size_t *)src;
	const TF_BYTE_TYPE *usx = (const TF_BYTE_TYPE *)src;
	size_t *dx = (size_t *)dst;
	TF_BYTE_TYPE *udx = (TF_BYTE_TYPE *)dst;
	size_t sl = sz;

	for (sl = 0; sl < (sz / sizeof(size_t)); sl++) dx[sl] ^= sx[sl];
	if (sz - (sl * sizeof(size_t))) for (sl *= sizeof(size_t); sl < sz; sl++) udx[sl] ^= usx[sl];
}

void tf_stream_crypt(struct tfe_stream *tfe, void *out, const void *in, size_t sz)
{
	tfe_emit(out, sz, tfe);
	xor_block(out, in, sz);
}
