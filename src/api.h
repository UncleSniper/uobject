#ifndef UOBJECT_API_H
#define UOBJECT_API_H

#ifdef __GNUC__
#define UOBJ_UNUSED(type, name) __attribute__((unused)) type name
#else /* unknown compiler */
#define UOBJ_UNUSED(type, name) type name
#endif

#endif /* UOBJECT_API_H */
