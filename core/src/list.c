#include <string.h>

#include "list.h"

void uobj_list_init(
	uobj_list_t *list,
	const uobj_list_callbacks_t *callbacks
) {
	list->callbacks = callbacks;
	list->size = (size_t)0u;
	list->head = list->tail = NULL;
}

uobj_list_t *uobj_list_new(
	const uobj_list_callbacks_t *callbacks,
	uobj_error_t *error
) {
	uobj_list_t *list;
	list = (uobj_list_t*)malloc(sizeof(uobj_list_t));
	if(!list) {
		*error = UOBJ_ERR_OUT_OF_MEMORY;
		return NULL;
	}
	uobj_list_init(list, callbacks);
	return list;
}

void uobj_list_destroy(
	const uobj_list_t *list
) {
	uobj_list_node_t *node, *next_node;
	const uobj_list_callbacks_t *callbacks;
	callbacks = list->callbacks;
	for(node = list->head; node; node = next_node) {
		next_node = node->next;
		if(callbacks->value_destructor)
			callbacks->value_destructor(&node->value);
		free(node);
	}
}

void uobj_list_delete(
	uobj_list_t *list
) {
	if(list) {
		uobj_list_destroy(list);
		free(list);
	}
}

uobj_error_t uobj_list_add_front(
	uobj_list_t *list,
	const uobj_variant_t *value
) {
	uobj_list_node_t *node;
	node = (uobj_list_node_t*)malloc(sizeof(uobj_list_node_t));
	if(!node)
		return UOBJ_ERR_OUT_OF_MEMORY;
	memcpy(&node->value, value, sizeof(uobj_variant_t));
	node->prev = NULL;
	node->next = list->head;
	if(list->head)
		list->head->prev = node;
	else
		list->tail = node;
	list->head = node;
	++list->size;
	return UOBJ_OK;
}

uobj_error_t uobj_list_add_back(
	uobj_list_t *list,
	const uobj_variant_t *value
) {
	uobj_list_node_t *node;
	node = (uobj_list_node_t*)malloc(sizeof(uobj_list_node_t));
	if(!node)
		return UOBJ_ERR_OUT_OF_MEMORY;
	memcpy(&node->value, value, sizeof(uobj_variant_t));
	node->prev = list->tail;
	node->next = NULL;
	if(list->tail)
		list->tail->next = node;
	else
		list->head = node;
	list->tail = node;
	++list->size;
	return UOBJ_OK;
}

static uobj_list_node_t *find_node_by_index(
	const uobj_list_t *list,
	size_t index
) {
	uobj_list_node_t *node;
	size_t from_back;
	if(index == list->size)
		return NULL;
	from_back = list->size - (size_t)1u - index;
	if(index < from_back) {
		for(node = list->head; index; --index)
			node = node->next;
	}
	else {
		for(node = list->tail; from_back; --from_back)
			node = node->prev;
	}
	return node;
}

uobj_error_t uobj_list_add_before(
	uobj_list_t *list,
	size_t index,
	const uobj_variant_t *value
) {
	uobj_list_node_t *anchor, *node;
	if(index > list->size)
		return UOBJ_ERR_INDEX_OUT_OF_BOUNDS;
	anchor = find_node_by_index(list, index);
	if(!anchor)
		return uobj_list_add_back(list, value);
	node = (uobj_list_node_t*)malloc(sizeof(uobj_list_node_t));
	if(!node)
		return UOBJ_ERR_OUT_OF_MEMORY;
	memcpy(&node->value, value, sizeof(uobj_variant_t));
	node->prev = anchor->prev;
	node->next = anchor;
	if(node->prev)
		node->prev->next = node;
	else
		list->head = node;
	anchor->prev = node;
	++list->size;
	return UOBJ_OK;
}

uobj_error_t uobj_list_get_front(
	const uobj_list_t *list,
	uobj_variant_t **value
) {
	if(!list->head) {
		*value = NULL;
		return UOBJ_ERR_NO_SUCH_ELEMENT;
	}
	*value = &list->head->value;
	return UOBJ_OK;
}

uobj_error_t uobj_list_get_back(
	const uobj_list_t *list,
	uobj_variant_t **value
) {
	if(!list->tail) {
		*value = NULL;
		return UOBJ_ERR_NO_SUCH_ELEMENT;
	}
	*value = &list->tail->value;
	return 1;
}

uobj_error_t uobj_list_get_at(
	const uobj_list_t *list,
	size_t index,
	uobj_variant_t **value
) {
	uobj_list_node_t *node;
	if(index >= list->size) {
		*value = NULL;
		return UOBJ_ERR_INDEX_OUT_OF_BOUNDS;
	}
	node = find_node_by_index(list, index);
	*value = &node->value;
	return UOBJ_OK;
}

uobj_error_t uobj_list_remove_front(
	uobj_list_t *list,
	uobj_variant_t *old_value
) {
	uobj_list_node_t *node;
	const uobj_list_callbacks_t *callbacks;
	node = list->head;
	if(!node)
		return UOBJ_ERR_NO_SUCH_ELEMENT;
	list->head = node->next;
	if(node->next)
		node->next->prev = NULL;
	else
		list->tail = NULL;
	callbacks = list->callbacks;
	if(old_value)
		memcpy(old_value, &node->value, sizeof(uobj_variant_t));
	else if(callbacks->value_destructor)
		callbacks->value_destructor(&node->value);
	free(node);
	--list->size;
	return UOBJ_OK;
}

uobj_error_t uobj_list_remove_back(
	uobj_list_t *list,
	uobj_variant_t *old_value
) {
	uobj_list_node_t *node;
	const uobj_list_callbacks_t *callbacks;
	node = list->tail;
	if(!node)
		return UOBJ_ERR_NO_SUCH_ELEMENT;
	list->tail = node->prev;
	if(node->prev)
		node->prev->next = NULL;
	else
		list->head = NULL;
	callbacks = list->callbacks;
	if(old_value)
		memcpy(old_value, &node->value, sizeof(uobj_variant_t));
	else if(callbacks->value_destructor)
		callbacks->value_destructor(&node->value);
	free(node);
	--list->size;
	return UOBJ_OK;
}

uobj_error_t uobj_list_remove_at(
	uobj_list_t *list,
	size_t index,
	uobj_variant_t *old_value
) {
	uobj_list_node_t *node;
	const uobj_list_callbacks_t *callbacks;
	if(index >= list->size)
		return UOBJ_ERR_INDEX_OUT_OF_BOUNDS;
	node = find_node_by_index(list, index);
	if(node->prev)
		node->prev->next = node->next;
	else
		list->head = node->next;
	if(node->next)
		node->next->prev = node->prev;
	else
		list->tail = node->prev;
	callbacks = list->callbacks;
	if(old_value)
		memcpy(old_value, &node->value, sizeof(uobj_variant_t));
	else if(callbacks->value_destructor)
		callbacks->value_destructor(&node->value);
	free(node);
	--list->size;
	return UOBJ_OK;
}

void uobj_list_clear(
	uobj_list_t *list
) {
	uobj_list_destroy(list);
	list->size = (size_t)0u;
	list->head = list->tail = NULL;
}

void uobj_list_iterator_init(
	uobj_list_iterator_t *iterator,
	const uobj_list_t *list
) {
	iterator->list = list;
	iterator->node = NULL;
}

uobj_list_iterator_t *uobj_list_iterator_new(
	const uobj_list_t *list,
	uobj_error_t *error
) {
	uobj_list_iterator_t *iterator;
	iterator = (uobj_list_iterator_t*)malloc(sizeof(uobj_list_iterator_t));
	if(!iterator) {
		*error = UOBJ_ERR_OUT_OF_MEMORY;
		return NULL;
	}
	uobj_list_iterator_init(iterator, list);
	return iterator;
}

uobj_variant_t *uobj_list_iterator_get(
	const uobj_list_iterator_t *iterator,
	uobj_error_t *error
) {
	if(!iterator->node) {
		*error = UOBJ_ERR_NO_SUCH_ELEMENT;
		return NULL;
	}
	return &iterator->node->value;
}

uobj_error_t uobj_list_iterator_next(
	uobj_list_iterator_t *iterator,
	uobj_variant_t **value
) {
	uobj_list_node_t *next;
	if(iterator->node)
		next = iterator->node->next;
	else
		next = iterator->list->head;
	iterator->node = next;
	if(!next) {
		if(value)
			*value = NULL;
		return UOBJ_ERR_NO_SUCH_ELEMENT;
	}
	if(value)
		*value = &next->value;
	return UOBJ_OK;
}

uobj_error_t uobj_list_iterator_previous(
	uobj_list_iterator_t *iterator,
	uobj_variant_t **value
) {
	uobj_list_node_t *prev;
	if(iterator->node)
		prev = iterator->node->prev;
	else
		prev = iterator->list->tail;
	iterator->node = prev;
	if(!prev) {
		if(value)
			*value = NULL;
		return UOBJ_ERR_NO_SUCH_ELEMENT;
	}
	if(value)
		*value = &prev->value;
	return UOBJ_OK;
}
