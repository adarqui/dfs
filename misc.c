#include "dfs.h"
 
#if !defined(MAXPATHLEN)
#define MAXPATHLEN  256
#endif

#ifndef CONST
#define CONST const
#endif

char *xdirname(CONST char *path)
{
    static char *bname;
    CONST char *endp;

    if (bname == NULL) {
        bname = (char *) malloc(MAXPATHLEN);
        if (bname == NULL)
            return (NULL);
    }

    /* Empty or NULL string gets treated as "." */
    if (path == NULL || *path == '\0') {
        strncpy(bname, ".", MAXPATHLEN);
        return (bname);
    }

    /* Strip trailing slashes */
    endp = path + strlen(path) - 1;
    while (endp > path && *endp == '/')
        endp--;

    /* Find the start of the dir */
    while (endp > path && *endp != '/')
        endp--;

    /* Either the dir is "/" or there are no slashes */
    if (endp == path) {
        strncpy(bname, *endp == '/' ? "/" : ".", MAXPATHLEN);
        return (bname);
    }

    do {
        endp--;
    } while (endp > path && *endp == '/');

    if (endp - path + 2 > MAXPATHLEN) {
        errno = ENAMETOOLONG;
        return (NULL);
    }
    strcpy(bname, path);
    bname[(endp - path) + 1] = 0;

    return (bname);
}
