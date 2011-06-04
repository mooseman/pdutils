/* ladsh2.c -- with i/o redirection */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_COMMAND_LEN 250     /* max length of a single command 
                                   string */
#define JOB_STATUS_FORMAT "[%d] %-22s %.40s\n"

enum redirectionType { REDIRECT_INPUT, REDIRECT_OVERWRITE, REDIRECT_APPEND };

struct jobSet {
    struct job * head;      /* head of list of running jobs */
    struct job * fg;        /* current foreground job */
};

struct redirectionSpecifier {
    enum redirectionType type;  /* type of redirection */
    int fd;                 /* file descriptor being redirected */
    char * filename;        /* file to redirect fd to */
};

struct childProgram {
    pid_t pid;              /* 0 if exited */
    char ** argv;           /* program name and arguments */
    int numRedirections;    /* elements in redirection array */
    struct redirectionSpecifier * redirections;  /* I/O redirections */
};

struct job {
    int jobId;              /* job number */
    int numProgs;           /* total number of programs in job */
    int runningProgs;       /* number of programs running */
    char * text;            /* name of job */
    char * cmdBuf;          /* buffer various argv's point into */
    pid_t pgrp;             /* process group ID for the job */
    struct childProgram * progs; /* array of programs in job */
    struct job * next;      /* to track background commands */
};

void freeJob(struct job * cmd) {
    int i;

    for (i = 0; i < cmd->numProgs; i++) {
        free(cmd->progs[i].argv);
        if (cmd->progs[i].redirections) free(cmd->progs[i].redirections);
    }
    free(cmd->progs);
    if (cmd->text) free(cmd->text);
    free(cmd->cmdBuf);
}

int getCommand(FILE * source, char * command) {
    if (source == stdin) {
        printf("# ");
        fflush(stdout);
    }

    if (!fgets(command, MAX_COMMAND_LEN, source)) {
        if (source == stdin) printf("\n");
        return 1;
    }

    /* remove trailing newline */
    command[strlen(command) - 1] = '\0';

    return 0;
}

/* Return cmd->numProgs as 0 if no command is present (e.g. an empty
   line). If a valid command is found, commandPtr is set to point to
   the beginning of the next command (if the original command had more 
   then one job associated with it) or NULL if no more commands are 
   present. */
int parseCommand(char ** commandPtr, struct job * job, int * isBg) {
    char * command;
    char * returnCommand = NULL;
    char * src, * buf, * chptr;
    int argc = 0;
    int done = 0;
    int argvAlloced;
    int i;
    char quote = '\0';  
    int count;
    struct childProgram * prog;

    /* skip leading white space */
    while (**commandPtr && isspace(**commandPtr)) (*commandPtr)++;

    /* this handles empty lines and leading '#' characters */
        if (!**commandPtr || (**commandPtr=='#')) {
        job->numProgs = 0;
        *commandPtr = NULL;
        return 0;
    }

    *isBg = 0;
    job->numProgs = 1;
    job->progs = malloc(sizeof(*job->progs));

    /* We set the argv elements to point inside of this string. The 
       memory is freed by freeJob(). 

       Getting clean memory relieves us of the task of NULL 
       terminating things and makes the rest of this look a bit 
       cleaner (though it is, admittedly, a tad less efficient) */
    job->cmdBuf = command = calloc(1, strlen(*commandPtr) + 1);
    job->text = NULL;

    prog = job->progs;
    prog->numRedirections = 0;
    prog->redirections = NULL;

    argvAlloced = 5;
    prog->argv = malloc(sizeof(*prog->argv) * argvAlloced);
    prog->argv[0] = job->cmdBuf;

    buf = command;
    src = *commandPtr;
    while (*src && !done) {
        if (quote == *src) {
            quote = '\0';
        } else if (quote) {
            if (*src == '\\') {
                src++;
                if (!*src) {
                    fprintf(stderr, "character expected after \\\n");
                    freeJob(job);
                    return 1;
                }

                /* in shell, "\'" should yield \' */
                if (*src != quote) *buf++ = '\\';
            }
            *buf++ = *src;
        } else if (isspace(*src)) {
            if (*prog->argv[argc]) {
                buf++, argc++;
                /* +1 here leaves room for the NULL which ends argv */
                if ((argc + 1) == argvAlloced) {
                    argvAlloced += 5;
                    prog->argv = realloc(prog->argv, 
				    sizeof(*prog->argv) * argvAlloced);
                }
                prog->argv[argc] = buf;
            }
        } else switch (*src) {
          case '"':
          case '\'':
            quote = *src;
            break;

          case '#':                         /* comment */
            done = 1;
            break;

          case '>':                         /* redirections */
          case '<':
            i = prog->numRedirections++;
            prog->redirections = realloc(prog->redirections, 
                                sizeof(*prog->redirections) * (i + 1));

            prog->redirections[i].fd = -1;
            if (buf != prog->argv[argc]) {
                /* the stuff before this character may be the file number 
                   being redirected */
                prog->redirections[i].fd = strtol(prog->argv[argc], &chptr, 10);

                if (*chptr && *prog->argv[argc]) {
                    buf++, argc++;
                }
            }

            if (prog->redirections[i].fd == -1) {
                if (*src == '>')
                    prog->redirections[i].fd = 1;
                else
                    prog->redirections[i].fd = 0;
            }

            if (*src++ == '>') {
                if (*src == '>')
                    prog->redirections[i].type = REDIRECT_APPEND, src++;
                else 
                    prog->redirections[i].type = REDIRECT_OVERWRITE;
            } else {
                prog->redirections[i].type = REDIRECT_INPUT;
            }

            /* This isn't POSIX sh compliant. Oh well. */
            chptr = src;
            while (isspace(*chptr)) chptr++;

            if (!*chptr) {
                fprintf(stderr, "file name expected after %c\n", *src);
                freeJob(job);
                return 1;
            }

            prog->redirections[i].filename = buf;
            while (*chptr && !isspace(*chptr)) 
                *buf++ = *chptr++;

            src = chptr - 1;                /* we src++ later */
            prog->argv[argc] = ++buf;
            break;

          case '|':                         /* pipe */
            /* finish this command */
            if (*prog->argv[argc]) argc++;
            if (!argc) {
                fprintf(stderr, "empty command in pipe\n");
                freeJob(job);
                return 1;
            }
            prog->argv[argc] = NULL;

            /* and start the next */
            job->numProgs++;
            job->progs = realloc(job->progs, 
                                 sizeof(*job->progs) * job->numProgs);
            prog = job->progs + (job->numProgs - 1);
            prog->numRedirections = 0;
            prog->redirections = NULL;
            argc = 0;

            argvAlloced = 5;
            prog->argv = malloc(sizeof(*prog->argv) * argvAlloced);
            prog->argv[0] = ++buf;

            src++;
            while (*src && isspace(*src)) src++;

            if (!*src) {
                fprintf(stderr, "empty command in pipe\n");
                return 1;
            }
            src--;              /* we'll ++ it at the end of the loop */

            break;

          case '&':                         /* background */
            *isBg = 1;
          case ';':                         /* multiple commands */
            done = 1;
            returnCommand = *commandPtr + (src - *commandPtr) + 1;
            break;

          case '\\':
            src++;
            if (!*src) {
                freeJob(job);
                fprintf(stderr, "character expected after \\\n");
                return 1;
            }
            /* fallthrough */
          default:
            *buf++ = *src;
        }

        src++;
    }

    if (*prog->argv[argc]) {
        argc++;
    }
    if (!argc) {
        freeJob(job);
        return 0;
    }
    prog->argv[argc] = NULL;

    if (!returnCommand) {
        job->text = malloc(strlen(*commandPtr) + 1);
        strcpy(job->text, *commandPtr);
    } else {
        /* This leaves any trailing spaces, which is a bit sloppy */

        count = returnCommand - *commandPtr;
        job->text = malloc(count + 1);
        strncpy(job->text, *commandPtr, count);
        job->text[count] = '\0';
    }

    *commandPtr = returnCommand;

    return 0;
}

int setupRedirections(struct childProgram * prog) {
    int i;
    int openfd;
    int mode;
    struct redirectionSpecifier * redir = prog->redirections;

    for (i = 0; i < prog->numRedirections; i++, redir++) {
        switch (redir->type) {
          case REDIRECT_INPUT:
            mode = O_RDONLY;
            break;
          case REDIRECT_OVERWRITE:
            mode = O_RDWR | O_CREAT | O_TRUNC; 
            break;
          case REDIRECT_APPEND:
            mode = O_RDWR | O_CREAT | O_APPEND;
            break;
        }

        openfd = open(redir->filename, mode, 0666);
        if (openfd < 0) {
            /* this could get lost if stderr has been redirected, but
               bash and ash both lose it as well (though zsh doesn't!) */
            fprintf(stderr, "error opening %s: %s\n", redir->filename,
                        strerror(errno));
            return 1;
        }

        if (openfd != redir->fd) {
            dup2(openfd, redir->fd);
            close(openfd);
        }
    }

    return 0;
}

int runCommand(struct job newJob, struct jobSet * jobList, 
               int inBg) {
    struct job * job;
    int i;
    int nextin, nextout;
    int pipefds[2];             /* pipefd[0] is for reading */

    /* handle built-ins here -- we don't fork() so we can't background
       these very easily */
    if (!strcmp(newJob.progs[0].argv[0], "exit")) {
        /* this should return a real exit code */
        exit(0);
    } else if (!strcmp(newJob.progs[0].argv[0], "jobs")) {
        for (job = jobList->head; job; job = job->next)
            printf(JOB_STATUS_FORMAT, job->jobId, "Running", 
                    job->text);
        return 0;
    }

    nextin = 0, nextout = 1;
    for (i = 0; i < newJob.numProgs; i++) {
        if ((i + 1) < newJob.numProgs) {
            pipe(pipefds);
            nextout = pipefds[1];
        } else {
            nextout = 1;
        }

        if (!(newJob.progs[i].pid = fork())) {
            if (nextin != 0) {
                dup2(nextin, 0);
                close(nextin);
            }

            if (nextout != 1) {
                dup2(nextout, 1);
                close(nextout);
            }

            /* explicit redirections override pipes */
            setupRedirections(newJob.progs + i);

            execvp(newJob.progs[i].argv[0], newJob.progs[i].argv);
            fprintf(stderr, "exec() of %s failed: %s\n", 
                    newJob.progs[i].argv[0], 
                    strerror(errno));
            exit(1);
        }

        /* put our child in the process group whose leader is the
           first process in this pipe */
        setpgid(newJob.progs[i].pid, newJob.progs[0].pid);

        if (nextin != 0) close(nextin);
        if (nextout != 1) close(nextout);

        /* If there isn't another process, nextin is garbage 
           but it doesn't matter */
        nextin = pipefds[0];
    }

    newJob.pgrp = newJob.progs[0].pid;

    /* find the ID for the job to use */
    newJob.jobId = 1;
    for (job = jobList->head; job; job = job->next)
        if (job->jobId >= newJob.jobId)
            newJob.jobId = job->jobId + 1;

    /* add the job to the list of running jobs */
    if (!jobList->head) {
        job = jobList->head = malloc(sizeof(*job));
    } else {
        for (job = jobList->head; job->next; job = job->next);
        job->next = malloc(sizeof(*job));
        job = job->next;
    }

    *job = newJob;
    job->next = NULL;
    job->runningProgs = job->numProgs;

    if (inBg) {
        /* we don't wait for background jobs to return -- append it 
           to the list of backgrounded jobs and leave it alone */

        printf("[%d] %d\n", job->jobId, 
               newJob.progs[newJob.numProgs - 1].pid);
    } else {
        jobList->fg = job;

        /* move the new process group into the foreground */
        
        if (tcsetpgrp(0, newJob.pgrp))
            perror("tcsetpgrp");
    }

    return 0;
}

void removeJob(struct jobSet * jobList, struct job * job) {
    struct job * prevJob;

    freeJob(job); 
    if (job == jobList->head) {
        jobList->head = job->next;
    } else {
        prevJob = jobList->head;
        while (prevJob->next != job) prevJob = prevJob->next;
        prevJob->next = job->next;
    }

    free(job);
}

/* Checks to see if any background processes have exited -- if they 
   have, figure out why and see if a job has completed */
void checkJobs(struct jobSet * jobList) {
    struct job * job;
    pid_t childpid;
    int status;
    int progNum;
   
    while ((childpid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (job = jobList->head; job; job = job->next) {
            progNum = 0;
            while (progNum < job->numProgs && 
                        job->progs[progNum].pid != childpid)
                progNum++;
            if (progNum < job->numProgs) break;
        }

        job->runningProgs--;
        job->progs[progNum].pid = 0;

        if (!job->runningProgs) {
            printf(JOB_STATUS_FORMAT, job->jobId, "Done", job->text);
            removeJob(jobList, job);
        }
    }

    if (childpid == -1 && errno != ECHILD)
        perror("waitpid");
}

int main(int argc, char ** argv) {
    char command[MAX_COMMAND_LEN + 1];
    char * nextCommand = NULL;
    struct jobSet jobList = { NULL, NULL };
    struct job newJob;
    FILE * input = stdin;
    int i;
    int status;
    int inBg;

    if (argc > 2) {
        fprintf(stderr, "unexpected arguments; usage: ladsh1 "
                        "<commands>\n");
        exit(1);
    } else if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("fopen");
            exit(1);
        }
    }

    /* don't pay any attention to this signal; it just confuses 
       things and isn't really meant for shells anyway */
    signal(SIGTTOU, SIG_IGN);
    
    while (1) {
        if (!jobList.fg) {
            /* no job is in the foreground */

            /* see if any background processes have exited */
            checkJobs(&jobList);

            if (!nextCommand) {
                if (getCommand(input, command)) break;
                nextCommand = command;
            }

            if (!parseCommand(&nextCommand, &newJob, &inBg) &&
                              newJob.numProgs) {
                runCommand(newJob, &jobList, inBg);
            }
        } else {
            /* a job is running in the foreground; wait for it */
            i = 0;
            while (!jobList.fg->progs[i].pid) i++;

            waitpid(jobList.fg->progs[i].pid, &status, 0);

            jobList.fg->runningProgs--;
            jobList.fg->progs[i].pid = 0;
        
            if (!jobList.fg->runningProgs) {
                /* child exited */

                removeJob(&jobList, jobList.fg);
                jobList.fg = NULL;

                /* move the shell to the foreground */
                if (tcsetpgrp(0, getpid()))
                    perror("tcsetpgrp");
            }
        }
    }

    return 0;
}
