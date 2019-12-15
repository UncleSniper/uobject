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
	const uobj_list_callbacks_t *callbacks
) {
	uobj_list_t *list;
	list = (uobj_list_t*)malloc(sizeof(uobj_list_t));
	if(!list)
		return NULL;
	uobj_list_init(list, callbacks);
	return list;
}

void uobj_list_destroy(
	const uobj_list_t *list
);

void uobj_list_delete(
	uobj_list_t *list
);

int uobj_list_add_front(
	uobj_list_t *list,
	const uobj_variant_t *value
);

int uobj_list_add_back(
	uobj_list_t *list,
	const uobj_variant_t *value
);

int uobj_list_get_front(
	const uobj_list_t *list,
	uobj_variant_t **value
);

int uobj_list_get_back(
	const uobj_list_t *list,
	uobj_variant_t **value
);

int uobj_list_get_at(
	const uobj_list_t *list,
	size_t index,
	uobj_variant_t **value
);

int uobj_list_remove_front(
	uobj_list_t *list,
	uobj_variant_t *old_value
);

int uobj_list_remove_back(
	uobj_list_t *list,
	uobj_variant_t *old_value
);

int uobj_list_remove_at(
	uobj_list_t *list,
	size_t index,
	uobj_variant_t *old_value
);

void uobj_list_clear(
	uobj_list_t *list
);

void uobj_list_iterator_init(
	uobj_list_iterator_t *iterator,
	const uobj_list_t *list
);

uobj_list_iterator_t *uobj_list_iterator_new(
	const uobj_list_t *list
);

int uobj_list_iterator_next(
	uobj_list_iterator_t *iterator,
	uobj_variant_t **value
);

int uobj_list_iterator_previous(
	uobj_list_iterator_t *iterator,
	uobj_variant_t **value
);
