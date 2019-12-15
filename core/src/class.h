#ifndef UOBJECT_CLASS_H
#define UOBJECT_CLASS_H

#include "api.h"

struct uobj_context;
struct uobj_interface;

typedef struct uobj_class {
	struct uobj_context *UOBJ_CORE_CONST context;
	UOBJ_CORE_CONST char *UOBJ_CORE_CONST name;
} uobj_class_t;

int uobj_class_init(
	uobj_class_t *clazz,
	struct uobj_context *context,
	const char *name
);

uobj_class_t *uobj_class_new(
	struct uobj_context *context,
	const char *name
);

void uobj_class_destroy(
	const uobj_class_t *clazz
);

void uobj_class_delete(
	uobj_class_t *clazz
);

int uobj_class_unregister(
	const uobj_class_t *clazz
);

#endif /* UOBJECT_CLASS_H */
