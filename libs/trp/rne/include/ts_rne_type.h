#ifndef __TS_RNE_TYPE_H__
#define __TS_RNE_TYPE_H__

#include "ts_type.h"
#include <stdbool.h>

/* This must be placed around external function declaration for C++
 * support. */
#ifdef __cplusplus
#define BEGIN_DECLS extern "C" {
#define END_DECLS }
#else
#define BEGIN_DECLS
#define END_DECLS
#endif

BEGIN_DECLS

typedef bool TS_RNE_BOOL;
typedef TS_RNE_BOOL ts_rne_bool;

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#ifndef EPSILON
#define EPSILON 0.000001
#endif

#ifndef MASK_BITS
#define MASK_BITS(m) ((1ll << (m)) - 1)
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MAX_BIT
#define MAX_BIT(n) ((1ll << ((n)-1)) - 1)
#endif

#ifndef MIN_BIT
#define MIN_BIT(n) (((1ull << (64 - (n))) - 1) << (n))
#endif

#ifndef BLOB_ALIGN_BYTES
#define BLOB_ALIGN_BYTES (32 / 8)
#endif

#ifndef GET_ALIGNX_CEIL
#define GET_ALIGNX_CEIL(n, x) ((((n) + (x)-1) / (x)) * (x))
#endif

#ifndef GET_ALIGNX_FLOOR
#define GET_ALIGNX_FLOOR(n, x) (((n) / (x)) * (x))
#endif

#ifndef TS_FLOAT_BIT_NUM
#define TS_FLOAT_BIT_NUM 32
#endif

END_DECLS

#endif /* __TS_RNE_TYPE_H__ */
