#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <linux/limits.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

const unsigned int MODE_LENGTH = 10;
const unsigned int SIZE_UNITS = 1 << 10;
const unsigned int MAX_NAME_LENGTH = 1 << 5;
char buf[4096];

void print(char *name, struct stat *ls);
void recurse(char *l, struct stat *ls);
void convertModeFlags(unsigned int i, char *s);

int main(int argc, char** argv) {
    char* l = ".";
    if (argc != 1) {
        l = argv[1];
    }
    struct stat* ls;
    lstat(l, ls);
    recurse(l, ls);
}

void recurse(char *l, struct stat *ls) {
    DIR *d = opendir(l);
    print(l, ls);
    if (l[strlen(l) - 1] == '/') {
        l[strlen(l) - 1] = '\0';
    }
    struct dirent *dire;
    while ((dire = readdir(d)) != NULL) {
        char path[PATH_MAX];
        sprintf(path, "%s/%s", l, dire->d_name);
        lstat(path, ls);
        if (strcmp(dire->d_name, ".") == 0) {
            continue;
        }
        if (strcmp(dire->d_name, "..") == 0) {
            continue;
        }
        if (dire->d_type == DT_DIR) {
            recurse(path, ls);
            continue;
        }
        print(path, ls);
    }
}

void print(char *name, struct stat *ls) {
    // Get mode
    char mode[MODE_LENGTH + 1];
    convertModeFlags(ls->st_mode, mode);

    unsigned int inodeNumber = ls->st_ino;
    unsigned int size4k = ls->st_size / SIZE_UNITS;
    unsigned int nlink = ls->st_nlink;

    // Get user
    char user[MAX_NAME_LENGTH];
    //struct passwd* u = getpwuid(ls->st_uid);
    struct passwd* u = NULL;
    if (u != NULL) {
        strcpy(user, u->pw_name);
    } else {
        // Enough buffer to represent the int for sure
        sprintf(user, "%i", ls->st_uid);
    }

    // Get group
    char group[MAX_NAME_LENGTH];
    //struct group* g = getgrgid(ls->st_gid);
    struct group* g = NULL;
    if (g != NULL) {
        strcpy(group, g->gr_name);
    } else {
        sprintf(group, "%i", ls->st_gid);
    }

    unsigned int size = ls->st_size;

    // Sufficient space to store human readable mtime;
    //char *mtime = ctime_r(mktime(localtime(&ls->st_mtim.tv_sec)), buf);
    char *mtime = "NOT_WORKING";

    // If it is a symlink
    if (mode[0] == 'l') {
        printf("%i %i %s %i %s %s %i %s %s -> %s\n", inodeNumber, size4k, mode, nlink, user, group, size, mtime, name, "LINK_TARGET");
    } else {
        printf("%i %i %s %i %s %s %i %s %s\n", inodeNumber, size4k, mode, nlink, user, group, size, mtime, name);
    }
}

void setPermissions(unsigned int perm, char *s) {
    if ((perm & 04) == 04) {
        s[0] = 'r';
    }
    if ((perm & 02) == 02) {
        s[1] = 'w';
    }
    if ((perm & 01) == 01) {
        s[2] = 'x';
    }
}

// s should be a string of length MODE_LENGTH
void convertModeFlags(unsigned int mode, char *s) {
    for (int i = 0; i < MODE_LENGTH; i++) {
        s[i] = '-';
    }
    s[MODE_LENGTH] = '\0';

    // Set file type
    switch (mode & S_IFMT) {
        case S_IFDIR:
            s[0] = 'd';
            break;
        case S_IFLNK:
            s[0] = 'l';
            break;
        case S_IFIFO:
            s[0] = 'p';
            break;
        case S_IFSOCK:
            s[0] = 's';
            break;
        case S_IFCHR:
            s[0] = 'c';
            break;
        case S_IFBLK:
            s[0] = 'b';
            break;
        default:
            break;
    }

    // Set user permissions
    setPermissions(mode >> 6, s + 1);

    // Set group permissions
    setPermissions(mode >> 3, s + 4);

    // Set other permissions
    setPermissions(mode, s + 7);
}
