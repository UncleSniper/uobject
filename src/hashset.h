#ifndef UOBJECT_HASHSET_H
#define UOBJECT_HASHSET_H

#include "hashtable.h"

typedef struct uobj_hashset_node {
	uobj_variant_t value;
	struct uobj_hashset_node *next;
} uobj_hashset_node_t;

typedef struct uobj_hashset {
	uobj_hash_function_t hash;
	uobj_variant_comparator_t value_comparator;
	uobj_variant_destructor_t value_destructor;
	size_t modulus;
	size_t size;
	uobj_hashset_node_t **nodes;
	uobj_hashtable_used_link_t *links;
	size_t first_used;
} uobj_hashset_t;

typedef struct uobj_hashset_iterator {
	const uobj_hashset_t *set;
	size_t slot_index;
	uobj_hashset_node_t *node;
} uobj_hashset_iterator_t;

int uobj_hashset_init(
	uobj_hashset_t *set,
	uobj_hash_function_t hash,
	uobj_variant_comparator_t value_comparator,
	uobj_variant_destructor_t value_destructor,
	size_t modulus
);

#endif /* UOBJECT_HASHSET_H */
