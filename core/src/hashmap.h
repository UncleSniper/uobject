#ifndef UOBJECT_HASHMAP_H
#define UOBJECT_HASHMAP_H

#include "error.h"
#include "hashtable.h"

typedef struct uobj_hashmap_callbacks {
	uobj_hash_function_t hash;
	uobj_variant_comparator_t key_comparator;
	uobj_variant_destructor_t key_destructor;
	uobj_variant_destructor_t value_destructor;
} uobj_hashmap_callbacks_t;

typedef struct uobj_hashmap_node {
	UOBJ_CORE_CONST uobj_variant_t key;
	uobj_variant_t value;
	struct uobj_hashmap_node *UOBJ_CORE_CONST next;
} uobj_hashmap_node_t;

typedef struct uobj_hashmap {
	const uobj_hashmap_callbacks_t *UOBJ_CORE_CONST callbacks;
	UOBJ_CORE_CONST size_t modulus;
	UOBJ_CORE_CONST size_t size;
	uobj_hashmap_node_t *UOBJ_CORE_CONST *UOBJ_CORE_CONST nodes;
	uobj_hashtable_used_link_t *UOBJ_CORE_CONST links;
	UOBJ_CORE_CONST size_t first_used;
} uobj_hashmap_t;

typedef struct uobj_hashmap_iterator {
	const uobj_hashmap_t *UOBJ_CORE_CONST map;
	UOBJ_CORE_CONST size_t slot_index;
	uobj_hashmap_node_t *UOBJ_CORE_CONST node;
} uobj_hashmap_iterator_t;

uobj_error_t uobj_hashmap_init(
	uobj_hashmap_t *map,
	const uobj_hashmap_callbacks_t *callbacks,
	size_t modulus
);

uobj_hashmap_t *uobj_hashmap_new(
	const uobj_hashmap_callbacks_t *callbacks,
	size_t modulus,
	uobj_error_t *error
);

void uobj_hashmap_destroy(
	const uobj_hashmap_t *map
);

void uobj_hashmap_delete(
	uobj_hashmap_t *map
);

uobj_error_t uobj_hashmap_put(
	uobj_hashmap_t *map,
	const uobj_variant_t *key,
	const uobj_variant_t *value,
	uobj_variant_t *old_value
);

uobj_error_t uobj_hashmap_get(
	const uobj_hashmap_t *map,
	const uobj_variant_t *key,
	uobj_variant_t **value
);

int uobj_hashmap_has_key(
	const uobj_hashmap_t *map,
	const uobj_variant_t *key
);

uobj_error_t uobj_hashmap_remove(
	uobj_hashmap_t *map,
	const uobj_variant_t *key,
	uobj_variant_t *old_value
);

void uobj_hashmap_clear(
	uobj_hashmap_t *map
);

void uobj_hashmap_iterator_init(
	uobj_hashmap_iterator_t *iterator,
	const uobj_hashmap_t *map
);

uobj_hashmap_iterator_t *uobj_hashmap_iterator_new(
	const uobj_hashmap_t *map,
	uobj_error_t *error
);

uobj_error_t uobj_hashmap_iterator_next(
	uobj_hashmap_iterator_t *iterator,
	const uobj_variant_t **key,
	uobj_variant_t **value
);

#endif /* UOBJECT_HASHMAP_H */
