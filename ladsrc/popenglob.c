#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char ** argv) {
    char buf[1024];
    FILE * ls;
    int result;
    int i;

    strcpy(buf, "ls ");

    for (i = 1; i < argc; i++) {
        strcat(buf, argv[i]);
        strcat(buf, " ");
    }

    ls = popen(buf, "r");
    if (!ls) {
        perror("popen");
        return 1;
    }

    while (fgets(buf, sizeof(buf), ls))
        printf("%s", buf);

    result = pclose(ls);

    if (!WIFEXITED(result)) return 1;

    return 0;
}
