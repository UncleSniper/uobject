#ifndef UOBJECT_HASHTABLE_H
#define UOBJECT_HASHTABLE_H

#include "variant.h"

typedef struct uobj_hashtable_used_link {
	size_t prev;
	size_t next;
} uobj_hashtable_used_link_t;

typedef unsigned (*uobj_hash_function_t)(
	const uobj_variant_t *value
);

#endif /* UOBJECT_HASHTABLE_H */
