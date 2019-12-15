#include <errno.h>
#include <string.h>

#include "context.h"
#include "class.h"

int uobj_class_init(
	uobj_class_t *clazz,
	uobj_context_t *context,
	const char *name
) {
	clazz->context = context;
	if(name) {
		clazz->name = strdup(name);
		if(!clazz->name)
			return 0;
	}
	else
		clazz->name = NULL;
	return 1;
}

uobj_class_t *uobj_class_new(
	uobj_context_t *context,
	const char *name
) {
	uobj_class_t *clazz;
	clazz = (uobj_class_t*)malloc(sizeof(uobj_class_t));
	if(!clazz)
		return NULL;
	if(!uobj_class_init(clazz, context, name)) {
		free(clazz);
		return NULL;
	}
	return clazz;
}

void uobj_class_destroy(
	const uobj_class_t *clazz
) {
	if(clazz->context && uobj_context_has_class(clazz->context, clazz))
		uobj_class_unregister(clazz);
	if(clazz->name)
		free(clazz->name);
}

void uobj_class_delete(
	uobj_class_t *clazz
) {
	if(clazz) {
		uobj_class_destroy(clazz);
		free(clazz);
	}
}

int uobj_class_unregister(
	const uobj_class_t *clazz
) {
	uobj_context_t *context;
	uobj_variant_t key, value;
	context = clazz->context;
	if(!context) {
		errno = EINVAL;
		return 0;
	}
	if(!uobj_context_has_class(context, clazz)) {
		errno = ENOENT;
		return 0;
	}
	if(clazz->name) {
		key.opointer = (void*)clazz->name;
		uobj_hashmap_remove(&context->named_classes, &key, NULL);
	}
	key.opointer = (void*)clazz;
	uobj_hashset_remove(&context->known_classes, &key, &value);
	return 1;
}
