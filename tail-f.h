#ifndef ULUG_LSP_TAIL_F_H
#define ULUG_LSP_TAIL_F_H

#define TAIL_POSTPONE  1
#define TAIL_ERROR    -1
#define TAIL_OK        0

#include <sys/types.h>

/* File structure
 */

struct file {

    /* file descriptor */
    int fd;

    /* file name as string */
    char *fn;

    /* current file size */
    off_t size;

    /* is deallocated or not */
    int freed;
};

#endif  /* ULUG_LSP_TAIL_F_H */
