#ifndef UOBJECT_HASHSET_H
#define UOBJECT_HASHSET_H

#include "hashtable.h"

typedef struct uobj_hashset_callbacks {
	uobj_hash_function_t hash;
	uobj_variant_comparator_t value_comparator;
	uobj_variant_destructor_t value_destructor;
} uobj_hashset_callbacks_t;

typedef struct uobj_hashset_node {
	UOBJ_CORE_CONST uobj_variant_t value;
	struct uobj_hashset_node *UOBJ_CORE_CONST next;
} uobj_hashset_node_t;

typedef struct uobj_hashset {
	const uobj_hashset_callbacks_t *UOBJ_CORE_CONST callbacks;
	UOBJ_CORE_CONST size_t modulus;
	UOBJ_CORE_CONST size_t size;
	uobj_hashset_node_t *UOBJ_CORE_CONST *UOBJ_CORE_CONST nodes;
	uobj_hashtable_used_link_t *UOBJ_CORE_CONST links;
	UOBJ_CORE_CONST size_t first_used;
} uobj_hashset_t;

typedef struct uobj_hashset_iterator {
	const uobj_hashset_t *UOBJ_CORE_CONST set;
	UOBJ_CORE_CONST size_t slot_index;
	uobj_hashset_node_t *UOBJ_CORE_CONST node;
} uobj_hashset_iterator_t;

int uobj_hashset_init(
	uobj_hashset_t *set,
	const uobj_hashset_callbacks_t *callbacks,
	size_t modulus
);

uobj_hashset_t *uobj_hashset_new(
	const uobj_hashset_callbacks_t *callbacks,
	size_t modulus
);

void uobj_hashset_destroy(
	const uobj_hashset_t *set
);

void uobj_hashset_delete(
	uobj_hashset_t *set
);

int uobj_hashset_add(
	uobj_hashset_t *set,
	const uobj_variant_t *value
);

int uobj_hashset_get(
	const uobj_hashset_t *set,
	const uobj_variant_t *lookup_value,
	const uobj_variant_t **held_value
);

int uobj_hashset_contains(
	const uobj_hashset_t *set,
	const uobj_variant_t *value
);

int uobj_hashset_remove(
	uobj_hashset_t *set,
	const uobj_variant_t *value,
	uobj_variant_t *old_value
);

void uobj_hashset_clear(
	uobj_hashset_t *set
);

void uobj_hashset_iterator_init(
	uobj_hashset_iterator_t *iterator,
	const uobj_hashset_t *set
);

uobj_hashset_iterator_t *uobj_hashset_iterator_new(
	const uobj_hashset_t *set
);

int uobj_hashset_iterator_next(
	uobj_hashset_iterator_t *iterator,
	const uobj_variant_t **value
);

#endif /* UOBJECT_HASHSET_H */
