#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *lockSuffix = ".lock";
static const char *archiveSuffix = ".jar";

static bool exists(const char *filename)
{
    return access(filename, F_OK) != -1;
}

static bool endsWith(const char *string, const char *suffix)
{
    string += strlen(string) - strlen(suffix);
    return strcmp(string, suffix) == 0;
}

static char *strdupcat(const char *string1, const char *string2)
{
    size_t l1 = strlen(string1), l2 = strlen(string2);
    char *string = malloc(l1 + l2 + 1);
    memcpy(string, string1, l1);
    memcpy(string + l1, string2, l2);
    return string;
}

static const char *getArchiveFileName(char *argv[])
{
    for (; *argv; argv++)
        if (endsWith(*argv, archiveSuffix))
            return *argv;
    return NULL;
}

static const char *getLockFileName(char *argv[])
{
    const char *archiveFileName = getArchiveFileName(argv);
    if (archiveFileName)
        return strdupcat(archiveFileName, lockSuffix);
    return NULL;
}

static void attemptLock(const char *lockFileName)
{
    int fd = open(lockFileName, O_CREAT | O_WRONLY, 0666);
    if (!fd || flock(fd, LOCK_EX))
        err(EXIT_FAILURE, NULL);
}

static void replaceCOptionWithU(char *argv[])
{
    if (argv[1][0] == 'c')
        argv[1][0] == 'u';
}

int main(int argc, char *argv[])
{
    const char *archiveFileName = getArchiveFileName(argv);
    if (archiveFileName) {
        // The sequence of locking and existence check matters.
        // The existence check needs to be after locking to prevent a TOCTOU race condition.
        const char *lockFileName = getLockFileName(argv);
        if (lockFileName)
            attemptLock(lockFileName);
        else
            fprintf(stderr, "%s: warning: Could not determine lock file name, continueing without lock.\n", argv[0]);
        if (access(archiveFileName, F_OK) != -1)
            replaceCOptionWithU(argv);
    } else
        fprintf(stderr, "%s: warning: Could not determine archive file name, continueing without update magic and lock.\n", argv[0]);
    argv[0]++;  // ljar -> jar, skip l prefix. That allows users to create hardlinks to other librarians, too.
    execvp(argv[0], argv);
    err(EXIT_FAILURE, NULL);
}
