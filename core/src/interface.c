#include <string.h>

#include "context.h"
#include "interface.h"

uobj_error_t uobj_interface_init(
	uobj_interface_t *interface,
	uobj_context_t *context,
	const char *name
) {
	interface->context = context;
	if(name) {
		interface->name = strdup(name);
		if(!interface->name)
			return UOBJ_ERR_OUT_OF_MEMORY;
	}
	else
		interface->name = NULL;
	return UOBJ_OK;
}

uobj_interface_t *uobj_interface_new(
	uobj_context_t *context,
	const char *name,
	uobj_error_t *error
) {
	uobj_interface_t *interface;
	interface = (uobj_interface_t*)malloc(sizeof(uobj_interface_t));
	if(!interface) {
		*error = UOBJ_ERR_OUT_OF_MEMORY;
		return NULL;
	}
	*error = uobj_interface_init(interface, context, name);
	if(*error) {
		free(interface);
		return NULL;
	}
	return interface;
}

void uobj_interface_destroy(
	const uobj_interface_t *interface
) {
	if(interface->context && uobj_context_has_interface(interface->context, interface))
		uobj_interface_unregister(interface);
	if(interface->name)
		free(interface->name);
}

void uobj_interface_delete(
	uobj_interface_t *interface
) {
	if(interface) {
		uobj_interface_destroy(interface);
		free(interface);
	}
}

uobj_error_t uobj_interface_unregister(
	const uobj_interface_t *interface
) {
	uobj_context_t *context;
	uobj_variant_t key, value;
	context = interface->context;
	if(!context)
		return UOBJ_ERR_NOT_REGISTERED;
	if(!uobj_context_has_interface(context, interface))
		return UOBJ_ERR_NOT_REGISTERED;
	if(interface->name) {
		key.opointer = (void*)interface->name;
		uobj_hashmap_remove(&context->named_interfaces, &key, NULL);
	}
	key.opointer = (void*)interface;
	uobj_hashset_remove(&context->known_interfaces, &key, &value);
	return UOBJ_OK;
}
