/* phones.c -- simple phone database to illustrate Berkeley db */

/* This implements a very simple phone database. Full usage 
   information is given in the text. */

#include <alloca.h>
#include <db.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(void) {
    fprintf(stderr, "usage: phones -a [-f] <name> <phone>\n");
    fprintf(stderr, "              -d <name>\n");
    fprintf(stderr, "              -q <name>\n");
    fprintf(stderr, "              -l\n");
    exit(1);
}

/* Opens the database $HOME/.phonedb. If writeable is nonzero,
   the database is opened for updating. If writeable is 0, the
   database is opened read-only. An appropriate lock is put on
   the database in either case. */ 
DB * openDatabase(int writeable) {
    DB * db;
    char * filename;
    int flags;
    struct flock lock;

    /* Set up a lock on the entire file (l_len = 0 means
       lock the whole file) */
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    /* Set the open and lock modes */
    if (writeable) {
        flags = O_CREAT | O_RDWR;
        lock.l_type = F_WRLCK;
    } else {
        flags = O_RDONLY;
        lock.l_type = F_RDLCK;
    }

    filename = alloca(strlen(getenv("HOME")) + 20);
    strcpy(filename, getenv("HOME"));
    strcat(filename, "/.phonedb");

    db = dbopen(filename, flags, 0666, DB_HASH, NULL);
    if (!db) {
        fprintf(stderr, "failed to open %s: %s\n", filename, 
                strerror(errno));
        return NULL;
    }

    /* Now that the database is open, lock it */
    if (fcntl(db->fd(db), F_SETLK, &lock)) {
        if (errno == EAGAIN) 
            fprintf(stderr, "the phone database is already "
                            "locked\n");
        else
            fprintf(stderr, "failed to lock database: %s\n", 
                            strerror(errno));
        return NULL;
    }

    return db;
}

/* add a new record to the database; this parses the
   command-line arguments directly */
int addRecord(int argc, char ** argv) {
    DB * db;
    char * name, * phone;
    DBT key, value;
    int rc = 0;
    int overwrite = 0;
   
    /* check for our parameters; -f means overwrite an
       existing entry, and the name and phone number should
       be all that remains */
    if (!argc) usage();
    if (!strcmp(argv[0], "-f")) {
        overwrite = 1;
        argc--, argv++;
    }

    if (argc != 2) usage();

    name = argv[0];
    phone = argv[1];

    /* open the database for writing */
    if (!(db = openDatabase(1))) return 1;

    key.data = name;
    /* the +1 writes the trailing '\0' to the file */
    key.size = strlen(name) + 1;

    /* if we shouldn't overwrite an existing entry, check
       to see if this name is already used */
    if (!overwrite) {
        rc = db->get(db, &key, &value, 0);
        if (rc == -1) {
            fprintf(stderr, "get failed: %s\n", strerror(errno));
            rc = 1;
        } else if (!rc) {
            fprintf(stderr, "%s already listed as %s\n", name, 
                    (char *) value.data);
            rc = 1;
        } else {
            rc = 0 ;
        }
    }

    /* if everything has worked so far, update the database */
    if (!rc) {
        value.data = phone;
        value.size = strlen(phone) + 1;

        if (db->put(db, &key, &value, 0)) {
            fprintf(stderr, "put failed: %s\n", strerror(errno));
            rc = 1;
        }
    }

    db->close(db);

    return rc;
}

/* looks up a name, and prints the phone number associated
   with it; parses the command line directly */
int queryRecord(int argc, char ** argv) {
    DB * db;
    DBT key, value;
    int rc;

    /* only one argument is expected, a name to look up */
    if (argc != 1) usage();

    /* open the database for reading */
    if (!(db = openDatabase(0))) return 1;

    /* set up the key to look up */
    key.data = argv[0];
    key.size = strlen(argv[0]) + 1;

    rc = db->get(db, &key, &value, 0);
    if (rc == -1) {
        fprintf(stderr, "get failed: %s\n", strerror(errno));
        rc = 1;
    } else if (rc) {
        fprintf(stderr, "%s is not listed\n", argv[0]);
        rc = 1;
    } else {
        printf("%s %s\n", argv[0], (char *) value.data);
        rc = 0;
    }

    db->close(db);

    return rc;
}

/* delete the specified record; the name is passed as a
   command-line argument */
int delRecord(int argc, char ** argv) {
    DB * db;
    DBT key;
    int rc;

    /* only a single argument is expected */
    if (argc != 1) usage();

    /* open the database for updating */
    if (!(db = openDatabase(1))) return 1;

    /* set up the key */
    key.data = argv[0];
    key.size = strlen(argv[0]) + 1;

    rc = db->del(db, &key, 0);
    if (rc == -1) {
        fprintf(stderr, "del failed: %s\n", strerror(errno));
        rc = 1;
    } else if (rc) {
        fprintf(stderr, "%s is not listed\n", argv[0]);
        rc = 1;
    }

    db->close(db);

    return rc;
}

/* lists all of the records in the database */
int listRecords(void) {
    DB * db;
    DBT key, value;
    int rc;
    int flags = R_FIRST;

    /* open the database read-only */
    if (!(db = openDatabase(0))) return 1;

    /* iterate over all of the records */
    while (!(rc = db->seq(db, &key, &value, flags))) {
        flags = R_NEXT;
        printf("%s %s\n", (char *) key.data, (char *) value.data);
    }

    if (rc == -1) {
        fprintf(stderr, "seq failed: %s\n", strerror(errno));
        rc = 1;
    } else {
        rc = 0;
    }

    db->close(db);

    return rc;
}

int main(int argc, char ** argv) {
    if (argc == 1) usage();

    /* look for a mode flag, and call the appropriate function
       with the remainder of the arguments */
    if (!strcmp(argv[1], "-a")) 
        return addRecord(argc - 2, argv + 2);
    else if (!strcmp(argv[1], "-q")) 
        return queryRecord(argc - 2, argv + 2);
    else if (!strcmp(argv[1], "-d")) 
        return delRecord(argc - 2, argv + 2);
    else if (!strcmp(argv[1], "-l")) {
        if (argc != 2) usage();
        return listRecords();
    }

    usage();  /* did not recognize any options */
    return 0; /* doesn't get here due to usage() */
}
