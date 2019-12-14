#include <string.h>

#include "hashtable.h"

#define clamp(field) \
	unsigned uobj_variant_ ## field ## _hash( \
		const uobj_variant_t *value \
	) { \
		return (unsigned)value->field; \
	}
clamp(nchar)
clamp(schar)
clamp(uchar)
clamp(sshort)
clamp(ushort)
clamp(sint)
clamp(uint)
#undef clamp

#define ROT_UINT(value, by) (((value) << (by)) | ((value) >> ((unsigned)sizeof(unsigned) * 8u - (by))))

#define clamp(field) \
	unsigned uobj_variant_ ## field ## _hash( \
		const uobj_variant_t *value \
	) { \
		unsigned long fvalue; \
		unsigned lo, hi; \
		fvalue = (unsigned long)value->field; \
		lo = (unsigned)fvalue; \
		hi = (unsigned)(fvalue >> ((unsigned)sizeof(unsigned) * 8u)); \
		return ROT_UINT(lo, 7u) ^ hi; \
	}
clamp(slong)
clamp(ulong)
#undef clamp

#define SEGMENTS_FOR(type) ((unsigned)(sizeof(type) / sizeof(unsigned)))
#define SEGMENTS_SIZE(type) (SEGMENTS_FOR(type) * (unsigned)sizeof(unsigned))
#define REMAINDER_FOR(type) ((unsigned)sizeof(type) - SEGMENTS_SIZE(type))

#define clamp(field, type) \
	unsigned uobj_variant_ ## field ## _hash( \
		const uobj_variant_t *value \
	) { \
		unsigned parts[SEGMENTS_FOR(type)]; \
		unsigned u, all; \
		memcpy(parts, &value->field, (size_t)SEGMENTS_SIZE(type)); \
		all = 0u; \
		for(u = 0u; u < SEGMENTS_FOR(type); ++u) \
			all = ROT_UINT(all, 7u) ^ parts[u]; \
		return all; \
	}
clamp(sfloat, float)
clamp(dfloat, double)
clamp(lfloat, long double)
clamp(opointer, void*)
clamp(fpointer, void (*)())
#undef clamp

/* djb2 and sdbm: http://www.cse.yorku.ca/~oz/hash.html */

unsigned uobj_variant_zstring_hash_djb2(
	const uobj_variant_t *value
) {
	const unsigned char *str;
	unsigned hash, c;
	str = (const unsigned char*)value->opointer;
	hash = 5381u;
	while((c = (unsigned)*str++))
		hash = ((hash << 5u) + hash) + c;
	return hash;
}

unsigned uobj_variant_zstring_hash_sdbm(
	const uobj_variant_t *value
) {
	const unsigned char *str;
	unsigned hash, c;
	str = (const unsigned char*)value->opointer;
	hash = 0;
	while((c = (unsigned)*str++))
		hash = c + (hash << 6u) + (hash << 16u) - hash;
	return hash;
}
