#ifndef CCL_ARRAY_H
#define CCL_ARRAY_H


#include <stdint.h>

#define INSTANTIATE_DARRAY(struct_name, type) \
typedef struct struct_name {                  \
	type *items;                              \
	size_t count;                             \
	size_t capacity;                          \
} struct_name;

void DArrayResize_Impl(void *darray, size_t element_size, size_t new_capacity);
void DArrayExtend_Impl(void *darray, const void *buffer, size_t element_size, size_t element_count);
void DArrayFree_Impl(void *darray);
void DArrayClear_Impl(void *darray);


#define STATIC_ARRAY_LEN(array) (sizeof(array)/sizeof(*array))
#define DARRAY_ELEMENT_SIZE(darray) sizeof(*(darray).items)

#define DARRAY_INIT_CAPACITY 16

#define DArrayResize(darray_ptr, new_capacity) \
	DArrayResize_Impl((darray_ptr), DARRAY_ELEMENT_SIZE(*(darray_ptr)), new_capacity)

#define DArrayExtend(darray_ptr, buffer, count) \
	DArrayExtend_Impl((darray_ptr), buffer, DARRAY_ELEMENT_SIZE(*(darray_ptr)), count)

#define DArrayConcat(darray_ptr, darray_ptr_other) \
	DArrayExtend_Impl((darray_ptr), (darray_ptr_other)->items, DARRAY_ELEMENT_SIZE(*(darray_ptr)), (darray_ptr_other)->count)

#define DArrayAppend(darray_ptr, item)                                            \
do {                                                                              \
	while ((darray_ptr)->capacity < (darray_ptr)->count + 1)                      \
		DArrayResize((darray_ptr),                                                \
		             (darray_ptr)->capacity ? (darray_ptr)->capacity * 1.5f + 1 : \
					  DARRAY_INIT_CAPACITY);                                      \
	(darray_ptr)->items[(darray_ptr)->count++] = (item);                          \
} while(0)

#define DArrayExtendStatic(darray_ptr, static_array) \
	DArrayExtend(darray_ptr, (static_array), STATIC_ARRAY_LEN(static_array))

#define DArrayAppendMany(darray_ptr, type, ...) \
	DArrayExtendStatic(darray_ptr, ((type []){__VA_ARGS__}))

#define DArrayFree  DArrayFree_Impl
#define DArrayClear DArrayClear_Impl


#endif // CCL_ARRAY_H