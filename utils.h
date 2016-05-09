#ifndef __UTILS_H__
#define __UTILS_H__

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR_MSG(...) \
    do{ \
        fprintf(stderr,"%22s:%04u [ERROR]: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fputc('\n', stderr); \
    } while(0)

#define FAIL_MSG(x, ...) \
    do{ \
        if (!(x)) { \
            fprintf(stderr,"%22s:%04u [FAIL]: ", __FILE__, __LINE__); \
            fprintf(stderr, __VA_ARGS__); \
            fputc('\n', stderr); \
            goto fail; \
        } \
    } while(0)

#define FAIL(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%22s:%04u [FAIL]: %s\n", \
                    __FILE__, __LINE__, strerror(errno)); \
            goto fail; \
        } \
    } while(0) \

#define FATAL_MSG(x, ...) \
    do { \
        if (!(x)) { \
            fprintf(stderr,"%22s:%04u [FATAL]: ", __FILE__, __LINE__); \
            fprintf(stderr, __VA_ARGS__); \
            fputc('\n', stderr); \
            exit(EXIT_FAILURE); \
        } \
    } while(0) \

#define FATAL(x) \
    do { \
        if (!(x)) \
        { \
            fprintf(stderr, "%22s:%04u [FATAL]: %s\n", __FILE__, __LINE__, strerror(errno)); \
            exit(EXIT_FAILURE); \
        } \
    } while(0) \

#ifdef DEBUG
#define DMSG(...) do{fprintf(stderr, "%22s:%04u [DEBUG]: ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__);fputc('\n', stderr);}while(0)
#else
#define DMSG(...) do{}while(0)
#endif

#define SET_BIT(x, y) do {x |= 1 << y;} while(0)
#define CLEAR_BIT(x, y) do {x &= ~(1U << y);} while(0)
#define TOGGLE_BIT(x, y) do {x ^= 1 << y;} while(0)
#define GET_BIT(x, y) ((x & (1 << y)) >> y)

#endif /* __UTILS_H__ */
