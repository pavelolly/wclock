#ifndef STATIC_ASSERT_H
#define STATIC_ASSERT_H


#define _GLUE(a, b) a ## b
#define STATIC_ASSERT_GLUE(a, b) _GLUE(a, b)

#define STATIC_ASSERT(condition) \
    enum { STATIC_ASSERT_GLUE(static_assert_, __LINE__) = 1 / (condition) };


#endif // STATIC_ASSERT_H