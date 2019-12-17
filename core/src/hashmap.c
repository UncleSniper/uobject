#include <string.h>

#include "tweaks.h"
#include "hashmap.h"

#define SLOT_ARENA_CONTRIB (sizeof(uobj_hashmap_node_t*) + sizeof(uobj_hashtable_used_link_t))

uobj_error_t uobj_hashmap_init(
	uobj_hashmap_t *map,
	const uobj_hashmap_callbacks_t *callbacks,
	size_t modulus
) {
	size_t arena_size;
	if(!modulus)
		modulus = (size_t)UOBJECT_HASHMAP_DEFAULT_MODULUS;
	arena_size = modulus * SLOT_ARENA_CONTRIB;
	if(arena_size / SLOT_ARENA_CONTRIB != modulus)
		return UOBJ_ERR_MODULUS_OUT_OF_RANGE;
	map->nodes = (uobj_hashmap_node_t**)malloc(arena_size);
	if(!map->nodes)
		return UOBJ_ERR_OUT_OF_MEMORY;
	memset(map->nodes, 0, sizeof(uobj_hashmap_node_t*) * modulus);
	map->callbacks = callbacks;
	map->modulus = modulus;
	map->size = (size_t)0u;
	/* uobj_hashmap_node_t should be maximally aligned, so no alignment issue here */
	map->links = (uobj_hashtable_used_link_t*)(map->nodes + modulus);
	map->first_used = modulus;
	return UOBJ_OK;
}

uobj_hashmap_t *uobj_hashmap_new(
	const uobj_hashmap_callbacks_t *callbacks,
	size_t modulus,
	uobj_error_t *error
) {
	uobj_hashmap_t *map;
	map = (uobj_hashmap_t*)malloc(sizeof(uobj_hashmap_t));
	if(!map) {
		*error = UOBJ_ERR_OUT_OF_MEMORY;
		return NULL;
	}
	*error = uobj_hashmap_init(map, callbacks, modulus);
	if(*error) {
		free(map);
		return NULL;
	}
	return map;
}

void uobj_hashmap_destroy(
	const uobj_hashmap_t *map
) {
	size_t slot_index;
	uobj_hashmap_node_t *node, *next_node;
	const uobj_hashmap_callbacks_t *callbacks;
	callbacks = map->callbacks;
	for(slot_index = map->first_used; slot_index != map->modulus; slot_index = map->links[slot_index].next) {
		for(node = map->nodes[slot_index]; node; node = next_node) {
			if(callbacks->key_destructor)
				callbacks->key_destructor(&node->key);
			if(callbacks->value_destructor)
				callbacks->value_destructor(&node->value);
			next_node = node->next;
			free(node);
		}
	}
	free(map->nodes);
}

void uobj_hashmap_delete(
	uobj_hashmap_t *map
) {
	if(map) {
		uobj_hashmap_destroy(map);
		free(map);
	}
}

uobj_error_t uobj_hashmap_put(
	uobj_hashmap_t *map,
	const uobj_variant_t *key,
	const uobj_variant_t *value,
	uobj_variant_t *old_value
) {
	size_t slot_index;
	uobj_hashmap_node_t *node, **node_head;
	uobj_hashtable_used_link_t *link;
	const uobj_hashmap_callbacks_t *callbacks;
	callbacks = map->callbacks;
	slot_index = (size_t)callbacks->hash(key) % map->modulus;
	node_head = map->nodes + slot_index;
	for(node = *node_head; node; node = node->next) {
		if(!callbacks->key_comparator(key, &node->key))
			break;
	}
	if(node) {
		if(old_value)
			memcpy(old_value, &node->value, sizeof(uobj_variant_t));
		else if(callbacks->value_destructor)
			callbacks->value_destructor(&node->value);
		memcpy(&node->value, value, sizeof(uobj_variant_t));
		return UOBJ_ERR_ELEMENT_EXISTED;
	}
	node = (uobj_hashmap_node_t*)malloc(sizeof(uobj_hashmap_node_t));
	if(!node)
		return UOBJ_ERR_OUT_OF_MEMORY;
	memcpy(&node->key, key, sizeof(uobj_variant_t));
	memcpy(&node->value, value, sizeof(uobj_variant_t));
	node->next = *node_head;
	*node_head = node;
	if(!node->next) {
		link = map->links + slot_index;
		link->prev = map->modulus;
		link->next = map->first_used;
		if(link->next != map->modulus)
			map->links[link->next].prev = slot_index;
		map->first_used = slot_index;
	}
	++map->size;
	return UOBJ_OK;
}

uobj_error_t uobj_hashmap_get(
	const uobj_hashmap_t *map,
	const uobj_variant_t *key,
	uobj_variant_t **value
) {
	size_t slot_index;
	uobj_hashmap_node_t *node;
	const uobj_hashmap_callbacks_t *callbacks;
	callbacks = map->callbacks;
	slot_index = (size_t)callbacks->hash(key) % map->modulus;
	for(node = map->nodes[slot_index]; node; node = node->next) {
		if(!callbacks->key_comparator(key, &node->key))
			break;
	}
	if(!node) {
		if(value)
			*value = NULL;
		return UOBJ_ERR_NO_SUCH_ELEMENT;
	}
	if(value)
		*value = &node->value;
	return UOBJ_OK;
}

int uobj_hashmap_has_key(
	const uobj_hashmap_t *map,
	const uobj_variant_t *key
) {
	size_t slot_index;
	uobj_hashmap_node_t *node;
	const uobj_hashmap_callbacks_t *callbacks;
	callbacks = map->callbacks;
	slot_index = (size_t)callbacks->hash(key) % map->modulus;
	for(node = map->nodes[slot_index]; node; node = node->next) {
		if(!callbacks->key_comparator(key, &node->key))
			return 1;
	}
	return 0;
}

uobj_error_t uobj_hashmap_remove(
	uobj_hashmap_t *map,
	const uobj_variant_t *key,
	uobj_variant_t *old_value
) {
	size_t slot_index;
	uobj_hashmap_node_t *node, **node_ref;
	uobj_hashtable_used_link_t *link;
	const uobj_hashmap_callbacks_t *callbacks;
	callbacks = map->callbacks;
	slot_index = (size_t)callbacks->hash(key) % map->modulus;
	for(node_ref = map->nodes + slot_index; (node = *node_ref); node_ref = &node->next) {
		if(!callbacks->key_comparator(key, &node->key))
			break;
	}
	if(!node)
		return UOBJ_ERR_NO_SUCH_ELEMENT;
	if(callbacks->key_destructor)
		callbacks->key_destructor(&node->key);
	if(old_value)
		memcpy(old_value, &node->value, sizeof(uobj_variant_t));
	else if(callbacks->value_destructor)
		callbacks->value_destructor(&node->value);
	*node_ref = node->next;
	free(node);
	if(!map->nodes[slot_index]) {
		link = map->links + slot_index;
		if(link->prev == map->modulus)
			map->first_used = link->next;
		else
			map->links[link->prev].next = link->next;
		if(link->next != map->modulus)
			map->links[link->next].prev = link->prev;
	}
	--map->size;
	return UOBJ_OK;
}

void uobj_hashmap_clear(
	uobj_hashmap_t *map
) {
	size_t slot_index;
	uobj_hashmap_node_t *node, *next_node;
	const uobj_hashmap_callbacks_t *callbacks;
	callbacks = map->callbacks;
	for(slot_index = map->first_used; slot_index != map->modulus; slot_index = map->links[slot_index].next) {
		for(node = map->nodes[slot_index]; node; node = next_node) {
			if(callbacks->key_destructor)
				callbacks->key_destructor(&node->key);
			if(callbacks->value_destructor)
				callbacks->value_destructor(&node->value);
			next_node = node->next;
			free(node);
		}
		map->nodes[slot_index] = NULL;
	}
	map->size = (size_t)0u;
	map->first_used = map->modulus;
}

void uobj_hashmap_iterator_init(
	uobj_hashmap_iterator_t *iterator,
	const uobj_hashmap_t *map
) {
	iterator->map = map;
	iterator->slot_index = map->first_used;
	iterator->node = map->first_used == map->modulus ? NULL : map->nodes[map->first_used];
}

uobj_hashmap_iterator_t *uobj_hashmap_iterator_new(
	const uobj_hashmap_t *map,
	uobj_error_t *error
) {
	uobj_hashmap_iterator_t *iterator;
	iterator = (uobj_hashmap_iterator_t*)malloc(sizeof(uobj_hashmap_iterator_t));
	if(!iterator) {
		*error = UOBJ_ERR_OUT_OF_MEMORY;
		return NULL;
	}
	uobj_hashmap_iterator_init(iterator, map);
	return iterator;
}

uobj_error_t uobj_hashmap_iterator_next(
	uobj_hashmap_iterator_t *iterator,
	const uobj_variant_t **key,
	uobj_variant_t **value
) {
	uobj_hashmap_node_t *node;
	const uobj_hashmap_t *map;
	node = iterator->node;
	if(!node) {
		if(key)
			*key = NULL;
		if(value)
			*value = NULL;
		return UOBJ_ERR_NO_SUCH_ELEMENT;
	}
	if(key)
		*key = &node->key;
	if(value)
		*value = &node->value;
	if(node->next)
		iterator->node = node->next;
	else {
		map = iterator->map;
		iterator->slot_index = map->links[iterator->slot_index].next;
		iterator->node = iterator->slot_index == map->modulus ? NULL : map->nodes[iterator->slot_index];
	}
	return UOBJ_OK;
}
