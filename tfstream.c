#include <string.h>
#include "tfdef.h"
#include "tfe.h"

void tf_stream_crypt(struct tfe_stream *tfe, void *out, const void *in, size_t sz)
{
	tfe_emit(out, sz, tfe);
	xor_block(out, in, sz);
}
