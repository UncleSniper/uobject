#include <errno.h>
#include <string.h>

#include "context.h"
#include "interface.h"

int uobj_interface_init(
	uobj_interface_t *interface,
	uobj_context_t *context,
	const char *name
) {
	interface->context = context;
	if(name) {
		interface->name = strdup(name);
		if(!interface->name)
			return 0;
	}
	else
		interface->name = NULL;
	return 1;
}

uobj_interface_t *uobj_interface_new(
	uobj_context_t *context,
	const char *name
) {
	uobj_interface_t *interface;
	interface = (uobj_interface_t*)malloc(sizeof(uobj_interface_t));
	if(!interface)
		return NULL;
	if(!uobj_interface_init(interface, context, name)) {
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

int uobj_interface_unregister(
	const uobj_interface_t *interface
) {
	uobj_context_t *context;
	uobj_variant_t key, value;
	context = interface->context;
	if(!context) {
		errno = EINVAL;
		return 0;
	}
	if(!uobj_context_has_interface(context, interface)) {
		errno = ENOENT;
		return 0;
	}
	if(interface->name) {
		key.opointer = (void*)interface->name;
		uobj_hashmap_remove(&context->named_interfaces, &key, NULL);
	}
	key.opointer = (void*)interface;
	uobj_hashset_remove(&context->known_interfaces, &key, &value);
	return 1;
}
