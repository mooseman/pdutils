/* gather.c -- sample of scatter/gather write */
#include <sys/uio.h>

int main(void) {
    struct iovec buffers[3];

    buffers[0].iov_base = "hello";
    buffers[0].iov_len = 5;

    buffers[1].iov_base = " ";
    buffers[1].iov_len = 1;

    buffers[2].iov_base = "world\n";
    buffers[2].iov_len = 6;

    writev(1, buffers, 3);

    return 0;
}
