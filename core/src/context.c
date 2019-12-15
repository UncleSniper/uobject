#include <errno.h>

#include "tweaks.h"
#include "context.h"

const uobj_hashset_callbacks_t uobj_context_default_interface_hashset_callbacks = {
	uobj_variant_opointer_hash,
	uobj_variant_opointer_comparator,
	uobj_context_destroy_interface_variant
};

const uobj_hashmap_callbacks_t uobj_context_default_interface_hashmap_callbacks = {
	uobj_variant_zstring_hash_djb2,
	uobj_variant_zstring_comparator,
	uobj_variant_opointer_free_destructor,
	uobj_variant_nop_destructor
};

const uobj_hashset_callbacks_t uobj_context_default_class_hashset_callbacks = {
	uobj_variant_opointer_hash,
	uobj_variant_opointer_comparator,
	uobj_context_destroy_class_variant
};

const uobj_hashmap_callbacks_t uobj_context_default_class_hashmap_callbacks = {
	uobj_variant_zstring_hash_djb2,
	uobj_variant_zstring_comparator,
	uobj_variant_opointer_free_destructor,
	uobj_variant_nop_destructor
};

const uobj_context_config_t uobj_context_default_config = {
	(size_t)UOBJECT_CONTEXT_INTERFACE_HASHTABLE_DEFAULT_MODULUS,
	&uobj_context_default_interface_hashset_callbacks,
	(size_t)UOBJECT_CONTEXT_INTERFACE_HASHTABLE_DEFAULT_MODULUS,
	&uobj_context_default_interface_hashmap_callbacks,
	(size_t)UOBJECT_CONTEXT_CLASS_HASHTABLE_DEFAULT_MODULUS,
	&uobj_context_default_class_hashset_callbacks,
	(size_t)UOBJECT_CONTEXT_CLASS_HASHTABLE_DEFAULT_MODULUS,
	&uobj_context_default_class_hashmap_callbacks
};

void uobj_context_destroy_interface_variant(
	const uobj_variant_t *value
) {
	uobj_interface_delete((uobj_interface_t*)value->opointer);
}

void uobj_context_destroy_class_variant(
	const uobj_variant_t *value
) {
	uobj_class_delete((uobj_class_t*)value->opointer);
}

int uobj_context_init(
	uobj_context_t *context,
	const uobj_context_config_t *config
) {
	int error;
	if(!config)
		config = &uobj_context_default_config;
	if(!uobj_hashset_init(&context->known_interfaces, config->interface_hashset_callbacks
			? config->interface_hashset_callbacks : &uobj_context_default_interface_hashset_callbacks,
			config->interface_hashset_modulus))
		return 0;
	if(!uobj_hashmap_init(&context->named_interfaces, config->interface_hashmap_callbacks
			? config->interface_hashmap_callbacks : &uobj_context_default_interface_hashmap_callbacks,
			config->interface_hashmap_modulus)) {
		error = errno;
		uobj_hashset_destroy(&context->known_interfaces);
		errno = error;
		return 0;
	}
	if(!uobj_hashset_init(&context->known_classes, config->class_hashset_callbacks
			? config->class_hashset_callbacks : &uobj_context_default_class_hashset_callbacks,
			config->class_hashset_modulus)) {
		error = errno;
		uobj_hashset_destroy(&context->known_interfaces);
		uobj_hashmap_destroy(&context->named_interfaces);
		errno = error;
		return 0;
	}
	if(!uobj_hashmap_init(&context->named_classes, config->class_hashmap_callbacks
			? config->class_hashmap_callbacks : &uobj_context_default_class_hashmap_callbacks,
			config->class_hashmap_modulus)) {
		error = errno;
		uobj_hashset_destroy(&context->known_interfaces);
		uobj_hashmap_destroy(&context->named_interfaces);
		uobj_hashset_destroy(&context->known_classes);
		errno = error;
		return 0;
	}
	return 1;
}

uobj_context_t *uobj_context_new(
	const uobj_context_config_t *config
) {
	uobj_context_t *context;
	context = (uobj_context_t*)malloc(sizeof(uobj_context_t));
	if(!context)
		return NULL;
	if(!uobj_context_init(context, config)) {
		free(context);
		return NULL;
	}
	return context;
}

void uobj_context_destroy(
	const uobj_context_t *context
) {
	uobj_hashmap_destroy(&context->named_interfaces);
	uobj_hashset_destroy(&context->known_interfaces);
	uobj_hashmap_destroy(&context->named_classes);
	uobj_hashset_destroy(&context->known_classes);
}

void uobj_context_delete(
	uobj_context_t *context
) {
	if(context) {
		uobj_context_destroy(context);
		free(context);
	}
}

uobj_interface_t *uobj_context_register_interface(
	uobj_context_t *context,
	const char *name
) {
	uobj_interface_t *interface;
	uobj_variant_t key, value;
	int error;
	interface = uobj_interface_new(context, name);
	if(!interface)
		return NULL;
	value.opointer = (void*)interface;
	if(name) {
		key.opointer = (void*)name;
		if(uobj_hashmap_has_key(&context->named_interfaces, &key)) {
			uobj_interface_delete(interface);
			errno = EEXIST;
			return NULL;
		}
		if(!uobj_hashmap_put(&context->named_interfaces, &key, &value, NULL)) {
			error = errno;
			uobj_interface_delete(interface);
			errno = error;
			return NULL;
		}
	}
	if(!uobj_hashset_add(&context->known_interfaces, &value)) {
		error = errno;
		if(name)
			uobj_hashmap_remove(&context->named_interfaces, &key, NULL);
		uobj_interface_delete(interface);
		errno = error;
		return NULL;
	}
	return interface;
}

int uobj_context_has_interface(
	const uobj_context_t *context,
	const uobj_interface_t *interface
) {
	uobj_variant_t value;
	value.opointer = (void*)interface;
	return uobj_hashset_contains(&context->known_interfaces, &value);
}

uobj_interface_t *uobj_context_get_interface(
	const uobj_context_t *context,
	const char *name
) {
	uobj_variant_t key, *value;
	key.opointer = (void*)name;
	if(!uobj_hashmap_get(&context->named_interfaces, &key, &value))
		return NULL;
	return (uobj_interface_t*)value->opointer;
}

uobj_class_t *uobj_context_register_class(
	uobj_context_t *context,
	const char *name
) {
	uobj_class_t *clazz;
	uobj_variant_t key, value;
	int error;
	clazz = uobj_class_new(context, name);
	if(!clazz)
		return NULL;
	value.opointer = (void*)clazz;
	if(name) {
		key.opointer = (void*)name;
		if(uobj_hashmap_has_key(&context->named_classes, &key)) {
			uobj_class_delete(clazz);
			errno = EEXIST;
			return NULL;
		}
		if(!uobj_hashmap_put(&context->named_classes, &key, &value, NULL)) {
			error = errno;
			uobj_class_delete(clazz);
			errno = error;
			return NULL;
		}
	}
	if(!uobj_hashset_add(&context->known_classes, &value)) {
		error = errno;
		if(name)
			uobj_hashmap_remove(&context->named_classes, &key, NULL);
		uobj_class_delete(clazz);
		errno = error;
		return NULL;
	}
	return clazz;
}

int uobj_context_has_class(
	const uobj_context_t *context,
	const uobj_class_t *clazz
) {
	uobj_variant_t value;
	value.opointer = (void*)clazz;
	return uobj_hashset_contains(&context->known_classes, &value);
}

uobj_class_t *uobj_context_get_class(
	const uobj_context_t *context,
	const char *name
) {
	uobj_variant_t key, *value;
	key.opointer = (void*)name;
	if(!uobj_hashmap_get(&context->named_classes, &key, &value))
		return NULL;
	return (uobj_class_t*)value->opointer;
}
