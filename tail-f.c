/* ULUG Linux System Programming Study
 *
 * 2장. 파일 입출력
 *  파일 입출력 프로그램을 작성해본다.
 *  - open(), read(), write(), close() 함수를 사용한다.
 *  - lseek() 함수를 사용한다.
 *  - O_NONBLOCK, O_ASYNC 옵션을 알아본다.
 *
 * tail-f
 *  tail 명령에 -f 옵션을 붙인 것과 유사하게 동작하는 프로그램.
 *  여러 파일을 인자로 받을 수 있다. 여러 파일의 변화를 출력해야
 *  하기 때문에 O_NONBLOCK 옵션이 필요할 것 같아서 연습 문제에
 *  적절할 것 같아 만들게 되었다.
 *
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "tail-f.h"
#include "debug.h"

static int
fstat_size(int fd, off_t *size);

static struct file *
open_files(int count, char **fn);

static void
close_file(struct file *file);

static void
destroy_file(struct file *file);

static int
readline_or_postpone(struct file *file, off_t size);

static void
puts_pretty(struct file *file, const char *buf);

int main(int argc, char **argv)
{
    int nfds = argc - 1;
    char **fds = argv + 1;

    struct file *f = open_files (nfds, fds);

    int i;
    struct stat stat;

    for (;;) {
        for (i = 0; i < nfds; i++) {
            if (f[i].fd == -1)
                continue;
            
            if (fstat(f[i].fd, &stat) == -1) {
                perrorf ("stat() failed: %s", f[i].fn);
                close_file (&f[i]);
                continue;
            }

            readline_or_postpone (&f[i], stat.st_size);
        }
        
        sleep (1);
    }

    for (i = 0; i < nfds; i++) {
        destroy_file (&f[i]);
    }
    free (f);
}

static struct file *
open_files(int count, char **fn)
{
    int i;

    struct file *f = calloc (count, sizeof (struct file));
    struct stat stat;
    
    for (i = 0; i < count; i++) {
        int fd = open (fn[i], O_NONBLOCK, O_RDONLY);

        f[i].fd = fd;
        f[i].size = 0;
        f[i].freed = 0;

        f[i].fn = strdup (fn[i]);
        
        if (fd == -1) {
            perrorf ("open() failed: %s", fn[i]);
            continue;
        }
        
        if (fstat(fd, &stat) == -1) {
            perrorf ("stat() failed: %s", fn[i]);
            close_file (&f[i]);
            continue;
        }

        f[i].size = stat.st_size;
    }

    return f;
}

static void
close_file(struct file *file)
{
    if (file->fd == -1)
        return;
    
    close (file->fd);
    file->fd == -1;
}

static void
destroy_file(struct file *file)
{
    close_file (file);
    free (file->fn);
}


static int
readline_or_postpone(struct file *file, off_t cur_size)
{
    off_t size = file->size;
    
    if (cur_size == size)
        return TAIL_POSTPONE;

    if (cur_size < size) {
        fprintf (stderr, "file truncated: %s\n", file->fn);
        return TAIL_ERROR;
    }

    off_t end = lseek (file->fd, size, SEEK_SET);
    if (end == -1)
        return TAIL_ERROR;

    // XXX - what is the most efficent buffer size? make test script
    char buf[2048 + 1];
    off_t i = size;

    off_t newline = 0;

    while (i < cur_size) {
        int interval = cur_size - i;

        if (interval > 2048)
            interval = 2048;

        ssize_t len = read (file->fd, buf, interval);

        if (len == 0)
            break;
        
        if (len == -1) {
            extern int errno;
            
            if (errno == EAGAIN) {
                fputs("EAGAIN\n", stderr);
                return TAIL_POSTPONE;
            }

            perrorf ("read() failed: %s", file->fn);
        }

        char *c;
        buf[len] = '\0';
        
        if (c = strrchr (buf, '\n'))
            newline = i + (c - buf);

        assert (buf[c - buf] == '\n');
        
        i += len;
    }

    if (!newline) {
        fputs("nonewline\n", stderr);
        return TAIL_POSTPONE;
    }
    end = lseek(file->fd, size, SEEK_SET);
    if (end == -1)
        return TAIL_ERROR;

    i = size;
    newline++;
    
    while (i < newline) {
        int interval = newline - size;

        if (interval > 2048)
            interval = 2048;
        
        ssize_t len = read (file->fd, buf, interval);

        if (len == 0)
            break;
        
        if (len == -1) {
            extern int errno;
            
            if (errno == EAGAIN) {
                fputs("EAGAIN\n", stderr);

                return TAIL_POSTPONE;
            }
            perrorf ("read() failed: %s", file->fn);
        }

        buf[len] = '\0';
        puts_pretty (file, buf);
        
        i += len;
    }

    file->size = newline;

    return TAIL_OK;
}

static void
puts_pretty(struct file *file, const char *buf)
{
    static struct file *cur_file = NULL;

    if (cur_file != file) {
        cur_file = file;
        printf("\n==> %s <==\n\n", file->fn);
    }

    fputs(buf, stdout);
    fflush(stdout);
}
