#include <string.h>

#include "api.h"
#include "variant.h"

void uobj_variant_nop_destructor(
	UOBJ_UNUSED(const uobj_variant_t*, value)
) {
	/* nothing to do */
}

void uobj_variant_opointer_free_destructor(
	const uobj_variant_t *value
) {
	free(value->opointer);
}

#define clamp(field) \
	int uobj_variant_ ## field ## _comparator( \
		const uobj_variant_t *a, \
		const uobj_variant_t *b \
	) { \
		return (a->field > b->field) - (a->field < b->field); \
	}
clamp(nchar)
clamp(schar)
clamp(uchar)
clamp(sshort)
clamp(ushort)
clamp(sint)
clamp(uint)
clamp(slong)
clamp(ulong)
clamp(size)
clamp(sfloat)
clamp(dfloat)
clamp(lfloat)
clamp(opointer)
#undef clamp

int uobj_variant_zstring_comparator(
	const uobj_variant_t *a,
	const uobj_variant_t *b
) {
	return strcmp((const char*)a->opointer, (const char*)b->opointer);
}
