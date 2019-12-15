#ifndef UOBJECT_LIST_H
#define UOBJECT_LIST_H

#include "api.h"
#include "variant.h"

typedef struct uobj_list_callbacks {
	uobj_variant_destructor_t value_destructor;
} uobj_list_callbacks_t;

typedef struct uobj_list_node {
	uobj_variant_t value;
	struct uobj_list_node *UOBJ_CORE_CONST prev;
	struct uobj_list_node *UOBJ_CORE_CONST next;
} uobj_list_node_t;

typedef struct uobj_list {
	const uobj_list_callbacks_t *UOBJ_CORE_CONST callbacks;
	UOBJ_CORE_CONST size_t size;
	uobj_list_node_t *UOBJ_CORE_CONST head;
	uobj_list_node_t *UOBJ_CORE_CONST tail;
} uobj_list_t;

typedef struct uobj_list_iterator {
	const uobj_list_t *UOBJ_CORE_CONST list;
	uobj_list_node_t *UOBJ_CORE_CONST node;
} uobj_list_iterator_t;

void uobj_list_init(
	uobj_list_t *list,
	const uobj_list_callbacks_t *callbacks
);

uobj_list_t *uobj_list_new(
	const uobj_list_callbacks_t *callbacks
);

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

#endif /* UOBJECT_LIST_H */
