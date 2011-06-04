/* calc.c - calculator front-end which uses bc for everything */

/* This is a very simple calculator which uses the external bc
   command to do everything. It opens a pipe to bc, reads a command
   in, passes it to bc, and exits. */
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    char buf[1024];
    FILE * bc;
    int result;
  
    /* open a pipe to bc, and exit if we fail */
    bc = popen("bc", "w");
    if (!bc) {
        perror("popen");
        return 1;
    }

    /* prompt for an expression, and read it in */
    printf("expr: "); fflush(stdout);
    fgets(buf, sizeof(buf), stdin);

    /* send the expression to bc for evaluation */
    fprintf(bc, "%s\n", buf);
    fflush(bc);

    /* close the pipe to bc, and wait for it to exit */
    result = pclose(bc);

    if (!WIFEXITED(result))
        printf("(abnormal exit)\n");

    return 0;
}
