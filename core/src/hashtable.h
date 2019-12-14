#ifndef UOBJECT_HASHTABLE_H
#define UOBJECT_HASHTABLE_H

#include "api.h"
#include "variant.h"

typedef struct uobj_hashtable_used_link {
	UOBJ_CORE_CONST size_t prev;
	UOBJ_CORE_CONST size_t next;
} uobj_hashtable_used_link_t;

typedef unsigned (*uobj_hash_function_t)(
	const uobj_variant_t *value
);

#define UOBJ_CLAMP(field) \
	unsigned uobj_variant_ ## field ## _hash( \
		const uobj_variant_t *value \
	);
UOBJ_CLAMP(nchar)
UOBJ_CLAMP(schar)
UOBJ_CLAMP(uchar)
UOBJ_CLAMP(sshort)
UOBJ_CLAMP(ushort)
UOBJ_CLAMP(sint)
UOBJ_CLAMP(uint)
UOBJ_CLAMP(slong)
UOBJ_CLAMP(ulong)
UOBJ_CLAMP(size)
UOBJ_CLAMP(sfloat)
UOBJ_CLAMP(dfloat)
UOBJ_CLAMP(lfloat)
UOBJ_CLAMP(opointer)
#undef UOBJ_CLAMP

unsigned uobj_variant_zstring_hash_djb2(
	const uobj_variant_t *value
);

unsigned uobj_variant_zstring_hash_sdbm(
	const uobj_variant_t *value
);

#endif /* UOBJECT_HASHTABLE_H */
