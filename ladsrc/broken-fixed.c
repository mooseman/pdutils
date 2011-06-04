#include <stdlib.h>
#include <stdio.h>

char global[5];

int main(void) {
    char * dyn;
    char local[5];

    /* First, overwrite a buffer just a little bit */
    dyn = malloc(6);
    strcpy(dyn, "12345");
    printf("1: %s\n", dyn);
    free(dyn);

    /* Now overwrite the buffer a lot */
    dyn = malloc(9);
    strcpy(dyn, "12345678");
    printf("2: %s\n", dyn);

    /* Walk past the beginning of a malloced buffer */
    *(dyn - 1) = '\0';
    printf("3: %s\n", dyn);
    /* note we didn't free the pointer! */

    /* Now go after a local variable */
    strcpy(local, "12345");
    printf("4: %s\n", local);
    local[-1] = '\0';
    printf("5: %s\n", local);

    /* Finally, attack global data space */
    strcpy(global, "12345");
    printf("6: %s\n", global);

    /* And write over the space before the global buffer */
    global[-1] = '\0';
    printf("7: %s\n", global);

    return 0;
}
