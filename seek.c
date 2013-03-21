/* ULUG Linux System Programming Study
 *
 * 2장. 파일 입출력
 *  파일 입출력 프로그램을 작성해본다.
 *
 * seek
 *  open(), lseek(), pread(), pwrite() 사용에 따라
 *  파일 오프셋이 어떻게 변화하는지 알아본다.
 */
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "debug.h"

static int copy();
static void current(int fd);

int main()
{
    if (copy() == -1)
        return EXIT_FAILURE;

    int fd = open("seek.bin", O_RDWR);
    current(fd);

    char buf[4];
    read(fd, &buf, 4 * sizeof *buf);
    current(fd);

    pread(fd, &buf, 4 * sizeof *buf, 10);
    current(fd);

    write(fd, &buf, 4 * sizeof *buf);
    current(fd);

    pread(fd, &buf, 4 * sizeof *buf, 20);
    current(fd);

    close(fd);
}

static void
current(int fd)
{
    off_t cur = lseek(fd, 0, SEEK_CUR);
    printf("current = %d\n", (unsigned int) cur);
}


static int
copy()
{
    char buf[512];
    int i;

    int in = open("/dev/urandom", O_RDONLY);
    
    if (in == -1) {
        perrorf("open /dev/urandom failed");
        return -1;
    }

    int out = open("seek.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (out == -1) {
        perrorf("open seek.bin failed");
        goto copy_end_in;
    }
    
    for (i = 0; i < 4; i++) {
        if (read(in, buf, 512) == -1) {
            perrorf("read()");
            goto copy_end;
        }
        if (write(out, buf, 512) == -1) {
            perrorf("write()");
            goto copy_end;
        }
    }

    return 0;
    
copy_end:
    close(out);
copy_end_in:
    close(in);
    return -1;
}
