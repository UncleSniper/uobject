#ifndef UOBJECT_VARIANT_H
#define UOBJECT_VARIANT_H

#include <stdlib.h>

typedef union uobj_variant {
	char nchar;
	signed char schar;
	unsigned char uchar;
	short sshort;
	unsigned short ushort;
	int sint;
	unsigned uint;
	long slong;
	unsigned long ulong;
	size_t size;
	float sfloat;
	double dfloat;
	long double lfloat;
	void *opointer;
	void (*fpointer)();
} uobj_variant_t;

typedef void (*uobj_variant_destructor_t)(
	const uobj_variant_t *value
);

void uobj_variant_nop_destructor(
	const uobj_variant_t *value
);

void uobj_variant_opointer_free_destructor(
	const uobj_variant_t *value
);

typedef int (*uobj_variant_comparator_t)(
	const uobj_variant_t *a,
	const uobj_variant_t *b
);

#define UOBJ_CLAMP(field) \
	int uobj_variant_ ## field ## _comparator( \
		const uobj_variant_t *a, \
		const uobj_variant_t *b \
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

int uobj_variant_zstring_comparator(
	const uobj_variant_t *a,
	const uobj_variant_t *b
);

#endif /* UOBJECT_VARIANT_H */
