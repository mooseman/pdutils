/* dbsimple.c -- simple db example */

/* this program opens a database, and then closes it immediately */

#include <db.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

int main(void) {
    DB * db;

    db = dbopen("test.db", O_RDWR | O_CREAT, 0666, DB_HASH, NULL);
    if (!db) {
        perror("dbopen");
        return 1;
    }

    db->close(db);

    return 0;
}
