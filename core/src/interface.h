#ifndef UOBJECT_INTERFACE_H
#define UOBJECT_INTERFACE_H

#include "api.h"
#include "error.h"

struct uobj_context;

typedef void uobj_vtable_t;

typedef struct uobj_interface {
	struct uobj_context *UOBJ_CORE_CONST context;
	UOBJ_CORE_CONST char *UOBJ_CORE_CONST name;
} uobj_interface_t;

uobj_error_t uobj_interface_init(
	uobj_interface_t *interface,
	struct uobj_context *context,
	const char *name
);

uobj_interface_t *uobj_interface_new(
	struct uobj_context *context,
	const char *name,
	uobj_error_t *error
);

void uobj_interface_destroy(
	const uobj_interface_t *interface
);

void uobj_interface_delete(
	uobj_interface_t *interface
);

uobj_error_t uobj_interface_unregister(
	const uobj_interface_t *interface
);

#endif /* UOBJECT_INTERFACE_H */
