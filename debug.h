#ifndef ULUG_LSP_DEBUG_H
#define ULUG_LSP_DEBUG_H

#define perrorf(...) {                          \
    extern int errno;                           \
    int err = errno;                            \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, ": %s\n", strerror (err));  \
}

#endif  /* ULUG_LSP_DEBUG_H */
