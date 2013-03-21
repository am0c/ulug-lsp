#ifndef ULUG_LSP_DEBUG_H
#define ULUG_LSP_DEBUG_H

#include <string.h>
#include <errno.h>

#define perrorf(...) {                          \
    int err = errno;                            \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, ": %s\n", strerror (err));  \
}

#endif  /* ULUG_LSP_DEBUG_H */
