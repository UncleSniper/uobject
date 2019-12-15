#ifndef UOBJECT_CONTEXT_H
#define UOBJECT_CONTEXT_H

#include "class.h"
#include "hashmap.h"
#include "hashset.h"
#include "interface.h"

typedef struct uobj_context_config {
	size_t interface_hashset_modulus;
	const uobj_hashset_callbacks_t *interface_hashset_callbacks;
	size_t interface_hashmap_modulus;
	const uobj_hashmap_callbacks_t *interface_hashmap_callbacks;
	size_t class_hashset_modulus;
	const uobj_hashset_callbacks_t *class_hashset_callbacks;
	size_t class_hashmap_modulus;
	const uobj_hashmap_callbacks_t *class_hashmap_callbacks;
} uobj_context_config_t;

typedef struct uobj_context {
	UOBJ_CORE_CONST uobj_hashset_t known_interfaces;
	UOBJ_CORE_CONST uobj_hashmap_t named_interfaces;
	UOBJ_CORE_CONST uobj_hashset_t known_classes;
	UOBJ_CORE_CONST uobj_hashmap_t named_classes;
} uobj_context_t;

extern const uobj_hashset_callbacks_t uobj_context_default_interface_hashset_callbacks;
extern const uobj_hashmap_callbacks_t uobj_context_default_interface_hashmap_callbacks;
extern const uobj_hashset_callbacks_t uobj_context_default_class_hashset_callbacks;
extern const uobj_hashmap_callbacks_t uobj_context_default_class_hashmap_callbacks;
extern const uobj_context_config_t uobj_context_default_config;

void uobj_context_destroy_interface_variant(
	const uobj_variant_t *value
);

void uobj_context_destroy_class_variant(
	const uobj_variant_t *value
);

int uobj_context_init(
	uobj_context_t *context,
	const uobj_context_config_t *config
);

uobj_context_t *uobj_context_new(
	const uobj_context_config_t *config
);

void uobj_context_destroy(
	const uobj_context_t *context
);

void uobj_context_delete(
	uobj_context_t *context
);

uobj_interface_t *uobj_context_register_interface(
	uobj_context_t *context,
	const char *name
);

int uobj_context_has_interface(
	const uobj_context_t *context,
	const uobj_interface_t *interface
);

uobj_interface_t *uobj_context_get_interface(
	const uobj_context_t *context,
	const char *name
);

uobj_class_t *uobj_context_register_class(
	uobj_context_t *context,
	const char *name
);

int uobj_context_has_class(
	const uobj_context_t *context,
	const uobj_class_t *class
);

uobj_class_t *uobj_context_get_class(
	const uobj_context_t *context,
	const char *name
);

#endif /* UOBJECT_CONTEXT_H */
