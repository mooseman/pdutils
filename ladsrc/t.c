#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd1, fd2;

    fd1 = open("/etc/passwd", O_RDONLY);
    fd2 = open("/etc/passwd", O_RDONLY);

    printf("first: %d second: %d\n", 
                lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));

    lseek(fd1, 20, SEEK_SET);

    printf("first: %d second: %d\n", 
                lseek(fd1, 0, SEEK_CUR), lseek(fd2, 0, SEEK_CUR));

    return 0;
}
