/* nftw_dir_tree.c
   Demonstrate the use of nftw(3). Walk though the directory tree specified
   on the command line (or the current working directory if no directory
   is specified on the command line), displaying an indented hierarchy
   of files in the tree. For each file, display:
      * a letter indicating the file type (using the same letters as "ls -l")
      * a string indicating the file type, as supplied by nftw()
      * the file's i-node number.
*/
#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <sys/types.h> /* Type definitions used by many programs */
#include <stdio.h>     /* Standard I/O functions */
#include <stdlib.h>    /* Prototypes of commonly used library functions,
                             plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>    /* Prototypes for many system calls */
#include <errno.h>     /* Declares errno and defines error constants */
#include <string.h>    /* Commonly used string-handling functions */
#include <pwd.h>
#include <grp.h>

static int /* Callback function called by ftw() */
dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    if (type == FTW_NS)
    { /* Could not stat() file */
        printf("?");
    }
    else
    {
        switch (sbuf->st_mode & S_IFMT)
        { /* Print file type */
        case S_IFREG:
            printf("-");
            break;
        case S_IFDIR:
            printf("d");
            break;
        case S_IFCHR:
            printf("c");
            break;
        case S_IFBLK:
            printf("b");
            break;
        case S_IFLNK:
            printf("l");
            break;
        case S_IFIFO:
            printf("p");
            break;
        case S_IFSOCK:
            printf("s");
            break;
        default:
            printf("?");
            break; /* Should never happen (on Linux) */
        }
    }

    if (type != FTW_NS)
        printf("%7ld ", (long)sbuf->st_ino);
    else
        printf("        ");

    printf(" %*s", 4 * ftwb->level, " ");  /* Indent suitably */
    printf("%s\n", &pathname[ftwb->base]); /* Print basename */
    return 0;                              /* Tell nftw() to continue */
}

static int /* Callback function called by ftw() */
dirTree2(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    // print the tree root
    //  Calculate the indentation based on the level in the directory hierarchy
    int indent = ftwb->level;

    // Print the indentation characters
    for (int i = 0; i < indent; i++)
    {
        putchar(' ');
        putchar(' ');
    }

    // Print the tree structure
    if (ftwb->level > 0)
    {
        printf("├─");
    }
    else
    {
        printf("└─");
    }

    // Check if it's a regular file
    if (type == FTW_F)
    {
        printf("[");

        // Extract the file permissions from the st_mode field
        mode_t permissions = sbuf->st_mode;

        // Print the file permissions in the format -rw-rw-r--
        printf((S_ISDIR(permissions)) ? "d" : "-");
        printf((permissions & S_IRUSR) ? "r" : "-");
        printf((permissions & S_IWUSR) ? "w" : "-");
        printf((permissions & S_IXUSR) ? "x" : "-");
        printf((permissions & S_IRGRP) ? "r" : "-");
        printf((permissions & S_IWGRP) ? "w" : "-");
        printf((permissions & S_IXGRP) ? "x" : "-");
        printf((permissions & S_IROTH) ? "r" : "-");
        printf((permissions & S_IWOTH) ? "w" : "-");
        printf((permissions & S_IXOTH) ? "x " : "- ");

        // printf("%s\n", fpath); // Print the file path
    }
    // Check if it's a regular file
    if (type == FTW_F)
    {
        // Extract the user ID from the st_uid field
        uid_t uid = sbuf->st_uid;

        // Get the user name associated with the UID
        struct passwd *pw = getpwuid(uid);
        if (pw != NULL)
        {
            // Print the user name and file path
            printf("%s ", pw->pw_name);
        }
        else
        {
            // If the user name cannot be retrieved, print UID instead
            printf("User ID: %d, File: %s\n", uid, pathname);
        }
    }
    // print the group name
    if (type == FTW_F)
    {
        struct group *grp;
        gid_t gid = sbuf->st_gid;

        grp = getgrgid(gid);
        if (grp != NULL)
        {
            printf("%s ", grp->gr_name);
        }
        else
        {
            printf("Unknown ");
        }
    }
    printf(" %*s", 4, " ");
    // print the size
    if (type == FTW_F)
    {
        printf("%lld ", (long long)sbuf->st_size);
        printf("]");
    }

    printf("%s\n", &pathname[ftwb->base]); /* Print basename */
    return 0;                              /* Tell nftw() to continue */
}
int main(int argc, char *argv[])
{
    int flags = 0;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (nftw(argv[1], dirTree2, 10, flags) == -1)
    {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
