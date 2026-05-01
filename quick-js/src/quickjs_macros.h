/*
 * QuickJS Javascript Engine - Macros Header
 *
 * Copyright (c) 2017-2026 Fabrice Bellard
 * Copyright (c) 2017-2024 Charlie Gordon
 * Copyright (c) 2023-2026 Ben Noordhuis
 * Copyright (c) 2023-2026 Saúl Ibarra Corretgé
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef QUICKJS_MACROS_H
#define QUICKJS_MACROS_H

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#define QUICKJS_NG 1


/* C function definition */
typedef enum JSCFunctionEnum {  /* XXX: should rename for namespace isolation */
    JS_CFUNC_generic,
    JS_CFUNC_generic_magic,
    JS_CFUNC_constructor,
    JS_CFUNC_constructor_magic,
    JS_CFUNC_constructor_or_func,
    JS_CFUNC_constructor_or_func_magic,
    JS_CFUNC_f_f,
    JS_CFUNC_f_f_f,
    JS_CFUNC_getter,
    JS_CFUNC_setter,
    JS_CFUNC_getter_magic,
    JS_CFUNC_setter_magic,
    JS_CFUNC_iterator_next,
} JSCFunctionEnum;


typedef enum JSPromiseHookType {
    JS_PROMISE_HOOK_INIT,     // emitted when a new promise is created
    JS_PROMISE_HOOK_BEFORE,   // runs right before promise.then is invoked
    JS_PROMISE_HOOK_AFTER,    // runs right after promise.then is invoked
    JS_PROMISE_HOOK_RESOLVE,  // not emitted for rejected promises
} JSPromiseHookType;


typedef enum JSTypedArrayEnum {
    JS_TYPED_ARRAY_UINT8C = 0,
    JS_TYPED_ARRAY_INT8,
    JS_TYPED_ARRAY_UINT8,
    JS_TYPED_ARRAY_INT16,
    JS_TYPED_ARRAY_UINT16,
    JS_TYPED_ARRAY_INT32,
    JS_TYPED_ARRAY_UINT32,
    JS_TYPED_ARRAY_BIG_INT64,
    JS_TYPED_ARRAY_BIG_UINT64,
    JS_TYPED_ARRAY_FLOAT16,
    JS_TYPED_ARRAY_FLOAT32,
    JS_TYPED_ARRAY_FLOAT64,
} JSTypedArrayEnum;


typedef enum JSPromiseStateEnum {
    // argument to JS_PromiseState() was not in fact a promise
    JS_PROMISE_NOT_A_PROMISE = -1,
    JS_PROMISE_PENDING       =  0,
    JS_PROMISE_FULFILLED,
    JS_PROMISE_REJECTED,
} JSPromiseStateEnum;

/* Helpers. */
#if defined(_WIN32) || defined(__CYGWIN__)
# define QUICKJS_NG_PLAT_WIN32 1
#endif /* defined(_WIN32) || defined(__CYGWIN__) */

#if defined(__GNUC__) || defined(__clang__)
# define QUICKJS_NG_CC_GNULIKE 1
#endif /* defined(__GNUC__) || defined(__clang__) */

/*
 * `JS_EXTERN` -- helper macro that must be used to mark the external
 * interfaces of libqjs.
 *
 * Define BUILDING_QJS_SHARED when building and USING_QJS_SHARED when using
 * shared libqjs.
 *
 * Windows note: The `__declspec` syntax is supported by both Clang and GCC.
 * If building qjs, the BUILDING_QJS_SHARED macro must be defined for libqjs
 * (and only for it) to properly export symbols.
 */
#ifdef QUICKJS_NG_PLAT_WIN32
# if defined(BUILDING_QJS_SHARED)
#  define JS_EXTERN __declspec(dllexport)
# elif defined(USING_QJS_SHARED)
#  define JS_EXTERN __declspec(dllimport)
# else
#  define JS_EXTERN /* nothing */
# endif
#else
# ifdef QUICKJS_NG_CC_GNULIKE
#  define JS_EXTERN __attribute__((visibility("default")))
# else
#  define JS_EXTERN /* nothing */
# endif
#endif /* QUICKJS_NG_PLAT_WIN32 */

/*
 * `JS_LIBC_EXTERN` -- helper macro that must be used to mark the extern
 * interfaces of quickjs-libc specifically.
 */
#if defined(QUICKJS_NG_BUILD) && !defined(QJS_BUILD_LIBC) && defined(QUICKJS_NG_PLAT_WIN32)
/*
 * We are building QuickJS-NG, quickjs-libc is a static library and we are on
 * Windows. Then, make sure to not export any interfaces.
 */
# define JS_LIBC_EXTERN /* nothing */
#else
/*
 * Otherwise, if we are either (1) not building QuickJS-NG, (2) libc is built as
 * a part of libqjs, or (3) we are not on Windows, define JS_LIBC_EXTERN to
 * JS_EXTERN.
 */
# define JS_LIBC_EXTERN JS_EXTERN
#endif

/*
 * `JS_MODULE_EXTERN` -- helper macro that must be used to mark `js_init_module`
 * and other public functions of the binary modules. See examples/ for examples
 * of the usage.
 *
 * Windows note: -DQUICKJS_NG_MODULE_BUILD must be set when building a binary
 * module to properly set __declspec.
 */
#ifdef QUICKJS_NG_PLAT_WIN32
# ifdef QUICKJS_NG_MODULE_BUILD
#  define JS_MODULE_EXTERN __declspec(dllexport)
# else
#  define JS_MODULE_EXTERN __declspec(dllimport)
# endif
#else
# ifdef QUICKJS_NG_CC_GNULIKE
#  define JS_MODULE_EXTERN __attribute__((visibility("default")))
# else
#  define JS_MODULE_EXTERN /* nothing */
# endif
#endif /* QUICKJS_NG_PLAT_WIN32 */

/* Borrowed from Folly */
#ifndef JS_PRINTF_FORMAT
/* Clang on Windows doesn't seem to support _Printf_format_string_ */
#if defined(_MSC_VER) && !defined(__clang__)
#include <sal.h>
#define JS_PRINTF_FORMAT _Printf_format_string_
#define JS_PRINTF_FORMAT_ATTR(format_param, dots_param)
#else
#define JS_PRINTF_FORMAT
#if !defined(__clang__) && defined(__GNUC__)
#define JS_PRINTF_FORMAT_ATTR(format_param, dots_param) \
  __attribute__((format(gnu_printf, format_param, dots_param)))
#else
#define JS_PRINTF_FORMAT_ATTR(format_param, dots_param) \
  __attribute__((format(printf, format_param, dots_param)))
#endif
#endif
#endif

#undef QUICKJS_NG_CC_GNULIKE
#undef QUICKJS_NG_PLAT_WIN32

/* Overridable purely for testing purposes; don't touch. */
#ifndef JS_NAN_BOXING
#if INTPTR_MAX < INT64_MAX
#define JS_NAN_BOXING 1 /* Use NAN boxing for 32bit builds. */
#endif
#endif

/* JSValue tag values */
#define JS_TAG_FIRST       -9 /* first negative tag */
#define JS_TAG_BIG_INT     -9
#define JS_TAG_SYMBOL      -8
#define JS_TAG_STRING      -7
#define JS_TAG_STRING_ROPE -6
#define JS_TAG_MODULE      -3 /* used internally */
#define JS_TAG_FUNCTION_BYTECODE -2 /* used internally */
#define JS_TAG_OBJECT      -1

#define JS_TAG_INT        0
#define JS_TAG_BOOL       1
#define JS_TAG_NULL       2
#define JS_TAG_UNDEFINED  3
#define JS_TAG_UNINITIALIZED 4
#define JS_TAG_CATCH_OFFSET 5
#define JS_TAG_EXCEPTION  6
#define JS_TAG_SHORT_BIG_INT 7
#define JS_TAG_FLOAT64    8

#if defined(JS_CHECK_JSVALUE)

#define JS_MKVAL(tag, val)       ((JSValue)((tag) | (intptr_t)(val) << 4))
#define JS_MKPTR(tag, ptr)       ((JSValue)((tag) | (intptr_t)(ptr)))
#define JS_VALUE_GET_NORM_TAG(v) ((int)((intptr_t)(v) & 15))
#define JS_VALUE_GET_TAG(v)      ((int)((intptr_t)(v) & 15))
#define JS_VALUE_GET_SHORT_BIG_INT(v) JS_VALUE_GET_INT(v)
#define JS_VALUE_GET_PTR(v)      ((void *)((intptr_t)(v) & ~15))
#define JS_VALUE_GET_INT(v)      ((int)((intptr_t)(v) >> 4))
#define JS_VALUE_GET_BOOL(v)     ((int)((intptr_t)(v) >> 4))
#define JS_VALUE_GET_FLOAT64(v)  ((double)((intptr_t)(v) >> 4))
#define JS_TAG_IS_FLOAT64(tag)   ((int)(tag) == JS_TAG_FLOAT64)
#define JS_NAN                   JS_MKVAL(JS_TAG_FLOAT64, 0)

#elif defined(JS_NAN_BOXING) && JS_NAN_BOXING

#define JS_VALUE_GET_TAG(v) (int)((v) >> 32)
#define JS_VALUE_GET_INT(v) (int)(v)
#define JS_VALUE_GET_BOOL(v) (int)(v)
#define JS_VALUE_GET_SHORT_BIG_INT(v) (int)(v)
#define JS_VALUE_GET_PTR(v) (void *)(intptr_t)(v)

#define JS_MKVAL(tag, val) (((uint64_t)(tag) << 32) | (uint32_t)(val))
#define JS_MKPTR(tag, ptr) (((uint64_t)(tag) << 32) | (uintptr_t)(ptr))

#define JS_FLOAT64_TAG_ADDEND (0x7ff80000 - JS_TAG_FIRST + 1) /* quiet NaN encoding */

#define JS_NAN (0x7ff8000000000000 - ((uint64_t)JS_FLOAT64_TAG_ADDEND << 32))

#define JS_TAG_IS_FLOAT64(tag) ((unsigned)((tag) - JS_TAG_FIRST) >= (JS_TAG_FLOAT64 - JS_TAG_FIRST))

/* same as JS_VALUE_GET_TAG, but return JS_TAG_FLOAT64 with NaN boxing */
static inline int JS_VALUE_GET_NORM_TAG(JSValue v)
{
    uint32_t tag;
    tag = JS_VALUE_GET_TAG(v);
    if (JS_TAG_IS_FLOAT64(tag))
        return JS_TAG_FLOAT64;
    else
        return tag;
}

#else /* !JS_NAN_BOXING */

#define JS_VALUE_GET_TAG(v) ((int32_t)(v).tag)
/* same as JS_VALUE_GET_TAG, but return JS_TAG_FLOAT64 with NaN boxing */
#define JS_VALUE_GET_NORM_TAG(v) JS_VALUE_GET_TAG(v)
#define JS_VALUE_GET_INT(v) ((v).u.int32)
#define JS_VALUE_GET_BOOL(v) ((v).u.int32)
#define JS_VALUE_GET_FLOAT64(v) ((v).u.float64)
#define JS_VALUE_GET_SHORT_BIG_INT(v) ((v).u.short_big_int)
#define JS_VALUE_GET_PTR(v) ((v).u.ptr)

/* msvc doesn't understand designated initializers without /std:c++20 */
#ifdef __cplusplus
#define JS_MKPTR(tag, ptr) JS_MKPTR(tag, ptr)
#define JS_MKVAL(tag, val) JS_MKVAL(tag, val)
#define JS_NAN             JS_MKNAN() /* alas, not a constant expression */
#else
#define JS_MKPTR(tag, p)   (JSValue){ (JSValueUnion){ .ptr = p }, tag }
#define JS_MKVAL(tag, val) (JSValue){ (JSValueUnion){ .int32 = val }, tag }
#define JS_NAN             (JSValue){ (JSValueUnion){ .float64 = NAN }, JS_TAG_FLOAT64 }
#endif

#define JS_TAG_IS_FLOAT64(tag) ((unsigned)(tag) == JS_TAG_FLOAT64)

#endif /* !JS_NAN_BOXING */

#define JS_VALUE_IS_BOTH_INT(v1, v2) ((JS_VALUE_GET_TAG(v1) | JS_VALUE_GET_TAG(v2)) == 0)
#define JS_VALUE_IS_BOTH_FLOAT(v1, v2) (JS_TAG_IS_FLOAT64(JS_VALUE_GET_TAG(v1)) && JS_TAG_IS_FLOAT64(JS_VALUE_GET_TAG(v2)))

#define JS_VALUE_HAS_REF_COUNT(v) ((unsigned)JS_VALUE_GET_TAG(v) >= (unsigned)JS_TAG_FIRST)

/* special values */
#define JS_NULL      JS_MKVAL(JS_TAG_NULL, 0)
#define JS_UNDEFINED JS_MKVAL(JS_TAG_UNDEFINED, 0)
#define JS_FALSE     JS_MKVAL(JS_TAG_BOOL, 0)
#define JS_TRUE      JS_MKVAL(JS_TAG_BOOL, 1)
#define JS_EXCEPTION JS_MKVAL(JS_TAG_EXCEPTION, 0)
#define JS_UNINITIALIZED JS_MKVAL(JS_TAG_UNINITIALIZED, 0)

/* flags for object properties */
#define JS_PROP_CONFIGURABLE  (1 << 0)
#define JS_PROP_WRITABLE      (1 << 1)
#define JS_PROP_ENUMERABLE    (1 << 2)
#define JS_PROP_C_W_E         (JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE | JS_PROP_ENUMERABLE)
#define JS_PROP_LENGTH        (1 << 3) /* used internally in Arrays */
#define JS_PROP_TMASK         (3 << 4) /* mask for NORMAL, GETSET, VARREF, AUTOINIT */
#define JS_PROP_NORMAL         (0 << 4)
#define JS_PROP_GETSET         (1 << 4)
#define JS_PROP_VARREF         (2 << 4) /* used internally */
#define JS_PROP_AUTOINIT       (3 << 4) /* used internally */

/* flags for JS_DefineProperty */
#define JS_PROP_HAS_SHIFT        8
#define JS_PROP_HAS_CONFIGURABLE (1 << 8)
#define JS_PROP_HAS_WRITABLE     (1 << 9)
#define JS_PROP_HAS_ENUMERABLE   (1 << 10)
#define JS_PROP_HAS_GET          (1 << 11)
#define JS_PROP_HAS_SET          (1 << 12)
#define JS_PROP_HAS_VALUE        (1 << 13)

/* throw an exception if false would be returned
   (JS_DefineProperty/JS_SetProperty) */
#define JS_PROP_THROW            (1 << 14)
/* throw an exception if false would be returned in strict mode
   (JS_SetProperty) */
#define JS_PROP_THROW_STRICT     (1 << 15)

#define JS_PROP_NO_ADD           (1 << 16) /* internal use */
#define JS_PROP_NO_EXOTIC        (1 << 17) /* internal use */
#define JS_PROP_DEFINE_PROPERTY  (1 << 18) /* internal use */
#define JS_PROP_REFLECT_DEFINE_PROPERTY (1 << 19) /* internal use */

#ifndef JS_DEFAULT_STACK_SIZE
#define JS_DEFAULT_STACK_SIZE (8 * 1024 * 1024)
#endif

/* JS_Eval() flags */
#define JS_EVAL_TYPE_GLOBAL   (0 << 0) /* global code (default) */
#define JS_EVAL_TYPE_MODULE   (1 << 0) /* module code */
#define JS_EVAL_TYPE_DIRECT   (2 << 0) /* direct call (internal use) */
#define JS_EVAL_TYPE_INDIRECT (3 << 0) /* indirect call (internal use) */
#define JS_EVAL_TYPE_MASK     (3 << 0)

#define JS_EVAL_FLAG_STRICT   (1 << 3) /* force 'strict' mode */
#define JS_EVAL_FLAG_UNUSED   (1 << 4) /* unused */
/* compile but do not run. The result is an object with a
   JS_TAG_FUNCTION_BYTECODE or JS_TAG_MODULE tag. It can be executed
   with JS_EvalFunction(). */
#define JS_EVAL_FLAG_COMPILE_ONLY (1 << 5)
/* don't include the stack frames before this eval in the Error() backtraces */
#define JS_EVAL_FLAG_BACKTRACE_BARRIER (1 << 6)
/* allow top-level await in normal script. JS_Eval() returns a
   promise. Only allowed with JS_EVAL_TYPE_GLOBAL */
#define JS_EVAL_FLAG_ASYNC (1 << 7)

// Debug trace system: the debug output will be produced to the dump stream (currently
// stdout) if dumps are enabled and JS_SetDumpFlags is invoked with the corresponding
// bit set.
#define JS_DUMP_BYTECODE_FINAL   0x01  /* dump pass 3 final byte code */
#define JS_DUMP_BYTECODE_PASS2   0x02  /* dump pass 2 code */
#define JS_DUMP_BYTECODE_PASS1   0x04  /* dump pass 1 code */
#define JS_DUMP_BYTECODE_HEX     0x10  /* dump bytecode in hex */
#define JS_DUMP_BYTECODE_PC2LINE 0x20  /* dump line number table */
#define JS_DUMP_BYTECODE_STACK   0x40  /* dump compute_stack_size */
#define JS_DUMP_BYTECODE_STEP    0x80  /* dump executed bytecode */
#define JS_DUMP_READ_OBJECT     0x100  /* dump the marshalled objects at load time */
#define JS_DUMP_FREE            0x200  /* dump every object free */
#define JS_DUMP_GC              0x400  /* dump the occurrence of the automatic GC */
#define JS_DUMP_GC_FREE         0x800  /* dump objects freed by the GC */
#define JS_DUMP_MODULE_RESOLVE 0x1000  /* dump module resolution steps */
#define JS_DUMP_PROMISE        0x2000  /* dump promise steps */
#define JS_DUMP_LEAKS          0x4000  /* dump leaked objects and strings in JS_FreeRuntime */
#define JS_DUMP_ATOM_LEAKS     0x8000  /* dump leaked atoms in JS_FreeRuntime */
#define JS_DUMP_MEM           0x10000  /* dump memory usage in JS_FreeRuntime */
#define JS_DUMP_OBJECTS       0x20000  /* dump objects in JS_FreeRuntime */
#define JS_DUMP_ATOMS         0x40000  /* dump atoms in JS_FreeRuntime */
#define JS_DUMP_SHAPES        0x80000  /* dump shapes in JS_FreeRuntime */

/* atom support */
#define JS_ATOM_NULL 0

#define JS_CALL_FLAG_CONSTRUCTOR (1 << 0)

#define JS_EVAL_OPTIONS_VERSION 1

#define JS_INVALID_CLASS_ID 0

/* Object Writer/Reader (currently only used to handle precompiled code) */
#define JS_WRITE_OBJ_BYTECODE  (1 << 0) /* allow function/module */
#define JS_WRITE_OBJ_BSWAP     (0)      /* byte swapped output (obsolete, handled transparently) */
#define JS_WRITE_OBJ_SAB       (1 << 2) /* allow SharedArrayBuffer */
#define JS_WRITE_OBJ_REFERENCE (1 << 3) /* allow object references to encode arbitrary object graph */
#define JS_WRITE_OBJ_STRIP_SOURCE  (1 << 4) /* do not write source code information */
#define JS_WRITE_OBJ_STRIP_DEBUG   (1 << 5) /* do not write debug information */

#define JS_READ_OBJ_BYTECODE  (1 << 0) /* allow function/module */
#define JS_READ_OBJ_ROM_DATA  (0)      /* avoid duplicating 'buf' data (obsolete, broken by ICs) */
#define JS_READ_OBJ_SAB       (1 << 2) /* allow SharedArrayBuffer */
#define JS_READ_OBJ_REFERENCE (1 << 3) /* allow object references */

#define JS_DEF_CFUNC          0
#define JS_DEF_CGETSET        1
#define JS_DEF_CGETSET_MAGIC  2
#define JS_DEF_PROP_STRING    3
#define JS_DEF_PROP_INT32     4
#define JS_DEF_PROP_INT64     5
#define JS_DEF_PROP_DOUBLE    6
#define JS_DEF_PROP_UNDEFINED 7
#define JS_DEF_OBJECT         8
#define JS_DEF_ALIAS          9
#define JS_DEF_PROP_SYMBOL   10
#define JS_DEF_PROP_BOOL     11

/* Note: c++ does not like nested designators */
#define JS_CFUNC_DEF(name, length, func1) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, 0, { .func = { length, JS_CFUNC_generic, { .generic = func1 } } } }
#define JS_CFUNC_DEF2(name, length, func1, prop_flags) { name, prop_flags, JS_DEF_CFUNC, 0, { .func = { length, JS_CFUNC_generic, { .generic = func1 } } } }
#define JS_CFUNC_MAGIC_DEF(name, length, func1, magic) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, magic, { .func = { length, JS_CFUNC_generic_magic, { .generic_magic = func1 } } } }
#define JS_CFUNC_SPECIAL_DEF(name, length, cproto, func1) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, 0, { .func = { length, JS_CFUNC_ ## cproto, { .cproto = func1 } } } }
#define JS_ITERATOR_NEXT_DEF(name, length, func1, magic) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, magic, { .func = { length, JS_CFUNC_iterator_next, { .iterator_next = func1 } } } }
#define JS_CGETSET_DEF(name, fgetter, fsetter) { name, JS_PROP_CONFIGURABLE, JS_DEF_CGETSET, 0, { .getset = { .get = { .getter = fgetter }, .set = { .setter = fsetter } } } }
#define JS_CGETSET_DEF2(name, fgetter, fsetter, prop_flags) { name, prop_flags, JS_DEF_CGETSET, 0, { .getset = { .get = { .getter = fgetter }, .set = { .setter = fsetter } } } }
#define JS_CGETSET_MAGIC_DEF(name, fgetter, fsetter, magic) { name, JS_PROP_CONFIGURABLE, JS_DEF_CGETSET_MAGIC, magic, { .getset = { .get = { .getter_magic = fgetter }, .set = { .setter_magic = fsetter } } } }
#define JS_PROP_STRING_DEF(name, cstr, prop_flags) { name, prop_flags, JS_DEF_PROP_STRING, 0, { .str = cstr } }
#define JS_PROP_INT32_DEF(name, val, prop_flags) { name, prop_flags, JS_DEF_PROP_INT32, 0, { .i32 = val } }
#define JS_PROP_INT64_DEF(name, val, prop_flags) { name, prop_flags, JS_DEF_PROP_INT64, 0, { .i64 = val } }
#define JS_PROP_DOUBLE_DEF(name, val, prop_flags) { name, prop_flags, JS_DEF_PROP_DOUBLE, 0, { .f64 = val } }
#define JS_PROP_U2D_DEF(name, val, prop_flags) { name, prop_flags, JS_DEF_PROP_DOUBLE, 0, { .u64 = val } }
#define JS_PROP_UNDEFINED_DEF(name, prop_flags) { name, prop_flags, JS_DEF_PROP_UNDEFINED, 0, { .i32 = 0 } }
#define JS_PROP_SYMBOL_DEF(name, val, prop_flags) { name, prop_flags, JS_DEF_PROP_SYMBOL, 0, { .i32 = val } }
#define JS_PROP_BOOL_DEF(name, val, prop_flags) { name, prop_flags, JS_DEF_PROP_BOOL, 0, { .i32 = val } }
#define JS_OBJECT_DEF(name, tab, len, prop_flags) { name, prop_flags, JS_DEF_OBJECT, 0, { .prop_list = { tab, len } } }
#define JS_ALIAS_DEF(name, from) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_ALIAS, 0, { .alias = { from, -1 } } }
#define JS_ALIAS_BASE_DEF(name, from, base) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_ALIAS, 0, { .alias = { from, base } } }

/* GetOwnPropertyNames flags */
#define JS_GPN_STRING_MASK  (1 << 0)
#define JS_GPN_SYMBOL_MASK  (1 << 1)
#define JS_GPN_PRIVATE_MASK (1 << 2)
/* only include the enumerable properties */
#define JS_GPN_ENUM_ONLY    (1 << 4)
/* set theJSPropertyEnum.is_enumerable field */
#define JS_GPN_SET_ENUM     (1 << 5)

/* Version */
#define QJS_VERSION_MAJOR 0
#define QJS_VERSION_MINOR 14
#define QJS_VERSION_PATCH 0
#define QJS_VERSION_SUFFIX ""

/* return the module specifier (allocated with js_malloc()) or std::ptr::null_mut::<()>() if
   exception */
typedef char *JSModuleNormalizeFunc(JSContext *ctx,
                                    const char *module_base_name,
                                    const char *module_name, void *opaque);
typedef char *JSModuleNormalizeFunc2(JSContext *ctx,
                                     const char *module_base_name,
                                     const char *module_name,
                                     JSValueConst attributes,
                                     void *opaque);
typedef JSModuleDef *JSModuleLoaderFunc(JSContext *ctx,
                                        const char *module_name, void *opaque);

/* module loader with import attributes support */
typedef JSModuleDef *JSModuleLoaderFunc2(JSContext *ctx,
                                         const char *module_name, void *opaque,
                                         JSValueConst attributes);

/* return -1 if exception, 0 if OK */
typedef int JSModuleCheckSupportedImportAttributes(JSContext *ctx, void *opaque,
                                                   JSValueConst attributes);


typedef void JSPromiseHook(JSContext *ctx, JSPromiseHookType type,
                           JSValueConst promise, JSValueConst parent_promise,
                           void *opaque);

typedef void JSHostPromiseRejectionTracker(JSContext *ctx, JSValueConst promise,
                                           JSValueConst reason,
                                           bool is_handled, void *opaque);

typedef void JS_MarkFunc(JSRuntime *rt, JSGCObjectHeader *gp);

typedef void JSClassFinalizer(JSRuntime *rt, JSValueConst val);
typedef void JSClassGCMark(JSRuntime *rt, JSValueConst val,
                           JS_MarkFunc *mark_func);
typedef JSValue JSClassCall(JSContext *ctx, JSValueConst func_obj,
                            JSValueConst this_val, int argc,
                            JSValueConst *argv, int flags);
typedef void JSFreeArrayBufferDataFunc(JSRuntime *rt, void *opaque, void *ptr);

typedef int JSModuleInitFunc(JSContext *ctx, JSModuleDef *m);
typedef JSValue JSJobFunc(JSContext *ctx, int argc, JSValueConst *argv);


typedef JSValue JSCFunction(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
typedef JSValue JSCFunctionMagic(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic);
typedef JSValue JSCFunctionData(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic, JSValueConst *func_data);
typedef JSValue JSCClosure(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic, void *opaque);

typedef void JSCClosureFinalizerFunc(void*);

#endif /* QUICKJS_MACROS_H */
