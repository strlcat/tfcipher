#include <string.h>
#include "tfdef.h"
#include "tfe.h"

void tf_stream_crypt(struct tfe_stream *tfe, void *out, const void *in, size_t sz)
{
	const TF_UNIT_TYPE *uin = in;
	TF_UNIT_TYPE *uout = out;
	const TF_BYTE_TYPE *uuin = in;
	TF_BYTE_TYPE *uuout = out;
	size_t n, z, x;

	switch (TF_SIZE_UNIT) {
		case 2: n = 1; break;
		case 4: n = 2; break;
		case 8: n = 3; break;
	}

	tfe_emit(out, sz, tfe);
	for (z = 0; z < (sz >> n); z++) uout[z] ^= uin[z];
	if (sz - (z << n)) for (x = (z << n); x < sz; x++) uuout[x] ^= uuin[x];
}
