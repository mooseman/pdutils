#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int usecsleep(int secs, int usecs) {
    struct timeval tv;

    tv.tv_sec = secs;
    tv.tv_usec = usecs;

    return select(0, NULL, NULL, NULL, &tv);
}

int main(){
    usecsleep(0, 500000);
}
