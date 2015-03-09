#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
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

static const char *getLockFileName(const char *archiveFileName)
{
    return strdupcat(archiveFileName, lockSuffix);
}

static void attemptLock(const char *lockFileName)
{
    int fd = open(lockFileName, O_CREAT | O_WRONLY, 0666);
    if (!fd || flock(fd, LOCK_EX))
        err(EXIT_FAILURE, NULL);
}

static void setCorrectActionOption(const char *archiveFileName, char *argv[])
{
    if (exists(archiveFileName)) {
        if (argv[1][0] == 'c')
            argv[1][0] = 'u';
    } else {
        if (argv[1][0] == 'u')
            argv[1][0] = 'c';
    }
}

static char *getDelegateProgramName(char *originalName)
{
    return "jar";
}

int main(int argc, char *argv[])
{
    const char *archiveFileName = getArchiveFileName(argv);
    if (archiveFileName) {
        // The sequence of locking (in attemptLock()) and existence check (in setCorrectActionOption()) matters.
        // The existence check needs to be after locking to prevent a TOCTOU race condition.
        attemptLock(getLockFileName(archiveFileName));
        setCorrectActionOption(archiveFileName, argv);
    } else
        warnx("warning: Could not determine archive file name, continueing without update magic and lock.");
    argv[0] = getDelegateProgramName(argv[0]);
    execvp(argv[0], argv);
    err(EXIT_FAILURE, "%s", argv[0]);
}
