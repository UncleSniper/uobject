#include <errno.h>
#include <string.h>

#include "tweaks.h"
#include "hashset.h"

#define SLOT_ARENA_CONTRIB (sizeof(uobj_hashset_node_t*) + sizeof(uobj_hashtable_used_link_t))

int uobj_hashset_init(
	uobj_hashset_t *set,
	const uobj_hashset_callbacks_t *callbacks,
	size_t modulus
) {
	size_t arena_size;
	if(!modulus)
		modulus = (size_t)UOBJECT_HASHSET_DEFAULT_MODULUS;
	arena_size = modulus * SLOT_ARENA_CONTRIB;
	if(arena_size / SLOT_ARENA_CONTRIB != modulus) {
		errno = EDOM;
		return 0;
	}
	set->nodes = (uobj_hashset_node_t**)malloc(arena_size);
	if(!set->nodes)
		return 0;
	memset(set->nodes, 0, sizeof(uobj_hashset_node_t*) * modulus);
	set->callbacks = callbacks;
	set->modulus = modulus;
	set->size = (size_t)0u;
	/* uobj_hashset_node_t should be maximally aligned, so no alignment issue here */
	set->links = (uobj_hashtable_used_link_t*)(set->nodes + modulus);
	set->first_used = modulus;
	return 1;
}

uobj_hashset_t *uobj_hashset_new(
	const uobj_hashset_callbacks_t *callbacks,
	size_t modulus
) {
	uobj_hashset_t *set;
	set = (uobj_hashset_t*)malloc(sizeof(uobj_hashset_t));
	if(!set)
		return NULL;
	if(uobj_hashset_init(set, callbacks, modulus)) {
		free(set);
		return NULL;
	}
	return set;
}

void uobj_hashset_destroy(
	const uobj_hashset_t *set
) {
	size_t slot_index;
	uobj_hashset_node_t *node, *next_node;
	const uobj_hashset_callbacks_t *callbacks;
	callbacks = set->callbacks;
	for(slot_index = set->first_used; slot_index != set->modulus; slot_index = set->links[slot_index].next) {
		for(node = set->nodes[slot_index]; node; node = next_node) {
			if(callbacks->value_destructor)
				callbacks->value_destructor(&node->value);
			next_node = node->next;
			free(node);
		}
	}
	free(set->nodes);
}

void uobj_hashset_delete(
	uobj_hashset_t *set
) {
	if(set) {
		uobj_hashset_destroy(set);
		free(set);
	}
}

int uobj_hashset_add(
	uobj_hashset_t *set,
	const uobj_variant_t *value
) {
	size_t slot_index;
	uobj_hashset_node_t *node, **node_head;
	uobj_hashtable_used_link_t *link;
	const uobj_hashset_callbacks_t *callbacks;
	callbacks = set->callbacks;
	slot_index = (size_t)callbacks->hash(value) % set->modulus;
	node_head = set->nodes + slot_index;
	for(node = *node_head; node; node = node->next) {
		if(!callbacks->value_comparator(value, &node->value)) {
			errno = EEXIST;
			return 0;
		}
	}
	node = (uobj_hashset_node_t*)malloc(sizeof(uobj_hashset_node_t));
	if(!node)
		return 0;
	memcpy(&node->value, value, sizeof(uobj_variant_t));
	node->next = *node_head;
	*node_head = node;
	if(!node->next) {
		link = set->links + slot_index;
		link->prev = set->modulus;
		link->next = set->first_used;
		if(link->next != set->modulus)
			set->links[link->next].prev = slot_index;
		set->first_used = slot_index;
	}
	++set->size;
	return 1;
}

int uobj_hashset_get(
	const uobj_hashset_t *set,
	const uobj_variant_t *lookup_value,
	const uobj_variant_t **held_value
) {
	size_t slot_index;
	uobj_hashset_node_t *node;
	const uobj_hashset_callbacks_t *callbacks;
	callbacks = set->callbacks;
	slot_index = (size_t)callbacks->hash(lookup_value) % set->modulus;
	for(node = set->nodes[slot_index]; node; node = node->next) {
		if(!callbacks->value_comparator(lookup_value, &node->value))
			break;
	}
	if(!node) {
		if(held_value)
			*held_value = NULL;
		errno = ENOENT;
		return 0;
	}
	if(held_value)
		*held_value = &node->value;
	return 1;
}

int uobj_hashset_contains(
	const uobj_hashset_t *set,
	const uobj_variant_t *value
) {
	size_t slot_index;
	uobj_hashset_node_t *node;
	const uobj_hashset_callbacks_t *callbacks;
	callbacks = set->callbacks;
	slot_index = (size_t)callbacks->hash(value) % set->modulus;
	for(node = set->nodes[slot_index]; node; node = node->next) {
		if(!callbacks->value_comparator(value, &node->value))
			return 1;
	}
	return 0;
}

int uobj_hashset_remove(
	uobj_hashset_t *set,
	const uobj_variant_t *value,
	uobj_variant_t *old_value
) {
	size_t slot_index;
	uobj_hashset_node_t *node, **node_ref;
	uobj_hashtable_used_link_t *link;
	const uobj_hashset_callbacks_t *callbacks;
	callbacks = set->callbacks;
	slot_index = (size_t)callbacks->hash(value) % set->modulus;
	for(node_ref = set->nodes + slot_index; (node = *node_ref); node_ref = &node->next) {
		if(!callbacks->value_comparator(value, &node->value))
			break;
	}
	if(!node) {
		errno = ENOENT;
		return 0;
	}
	if(old_value)
		memcpy(old_value, &node->value, sizeof(uobj_variant_t));
	else if(callbacks->value_destructor)
		callbacks->value_destructor(&node->value);
	*node_ref = node->next;
	free(node);
	if(!set->nodes[slot_index]) {
		link = set->links + slot_index;
		if(link->prev == set->modulus)
			set->first_used = link->next;
		else
			set->links[link->prev].next = link->next;
		if(link->next != set->modulus)
			set->links[link->next].prev = link->prev;
	}
	--set->size;
	return 1;
}

void uobj_hashset_clear(
	uobj_hashset_t *set
) {
	size_t slot_index;
	uobj_hashset_node_t *node, *next_node;
	const uobj_hashset_callbacks_t *callbacks;
	callbacks = set->callbacks;
	for(slot_index = set->first_used; slot_index != set->modulus; slot_index = set->links[slot_index].next) {
		for(node = set->nodes[slot_index]; node; node = next_node) {
			if(callbacks->value_destructor)
				callbacks->value_destructor(&node->value);
			next_node = node->next;
			free(node);
		}
		set->nodes[slot_index] = NULL;
	}
	set->size = (size_t)0u;
	set->first_used = set->modulus;
}

void uobj_hashset_iterator_init(
	uobj_hashset_iterator_t *iterator,
	const uobj_hashset_t *set
) {
	iterator->set = set;
	iterator->slot_index = set->first_used;
	iterator->node = set->first_used == set->modulus ? NULL : set->nodes[set->first_used];
}

uobj_hashset_iterator_t *uobj_hashset_iterator_new(
	const uobj_hashset_t *set
) {
	uobj_hashset_iterator_t *iterator;
	iterator = (uobj_hashset_iterator_t*)malloc(sizeof(uobj_hashset_iterator_t));
	if(!iterator)
		return NULL;
	uobj_hashset_iterator_init(iterator, set);
	return iterator;
}

int uobj_hashset_iterator_next(
	uobj_hashset_iterator_t *iterator,
	const uobj_variant_t **value
) {
	uobj_hashset_node_t *node;
	const uobj_hashset_t *set;
	node = iterator->node;
	if(!node) {
		if(value)
			*value = NULL;
		errno = ENOENT;
		return 0;
	}
	if(value)
		*value = &node->value;
	if(node->next)
		iterator->node = node->next;
	else {
		set = iterator->set;
		iterator->slot_index = set->links[iterator->slot_index].next;
		iterator->node = iterator->slot_index == set->modulus ? NULL : set->nodes[iterator->slot_index];
	}
	return 1;
}
