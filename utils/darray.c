#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "darray.h"

struct DArray {
	void *items;
	size_t count;
	size_t capacity;
};

void DArrayResize_Impl(void *darray, size_t element_size, size_t new_capacity) {
	if (!darray) {
		return;
	}

	struct DArray *_array = (struct DArray *)darray;

	_array->items = realloc(_array->items, new_capacity * element_size);

	if (!_array->items) {
		_array->count = 0;
		_array->capacity = 0;
		return;
	}

	if (_array->count > new_capacity) {
		_array->count = new_capacity;
	}
	_array->capacity = new_capacity;
}

void DArrayExtend_Impl(void *darray, const void *buffer, size_t element_size, size_t element_count) {
	if (!darray) {
		return;
	}

	struct DArray *_array = (struct DArray *)darray;

	while (_array->capacity < _array->count + element_count) {
		DArrayResize_Impl(_array, element_size, _array->capacity ? _array->capacity * 1.5f + 1 : DARRAY_INIT_CAPACITY);
	}

	memcpy((char *)_array->items + _array->count * element_size,
		buffer,
		element_count * element_size);
	
	_array->count += element_count;
}

void DArrayFree_Impl(void *darray) {
	free(((struct DArray *)darray)->items);
}

void DArrayClear_Impl(void *darray) {
	if (!darray) {
		return;
	}

	struct DArray *_array = (struct DArray *)darray;
	DArrayFree_Impl(_array);
	_array->count = 0;
	_array->capacity = 0;
}