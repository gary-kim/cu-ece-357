#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <linux/limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const unsigned int MODE_LENGTH = 10;
const unsigned int MAX_NAME_LENGTH = 1 << 5;

// Method for padding came from the GNU Findutils source code.
// https://git.savannah.gnu.org/cgit/findutils.git/tree/lib/listfile.c?id=5768a03ddfb5e18b1682e339d6cdd24ff721c510
static int inode_number_width = 8;
static int block_size_width = 6;
static int nlink_width = 3;
static int owner_width = 8;
static int group_width = 8;
static int file_size_width = 8;

int print(char *name, struct stat *ls);
int recurse(char *l, struct stat *ls);
void convertModeFlags(unsigned int mode, char *s);

int main(int argc, char **argv) {
  char *l = ".";
  if (argc != 1) {
    l = argv[1];
  }
  struct stat ls;
  if (lstat(l, &ls) != 0) {
    fprintf(stderr,
            "Ran into error trying to get information on \"%s\". err = %s\n", l,
            strerror(errno));
    return 0;
  }
  return recurse(l, &ls);
}

int recurse(char *l, struct stat *ls) {
  print(l, ls);
  DIR *d = opendir(l);
  if (d == NULL) {
    if (errno == EACCES) {
      // If the problem is simply a permissions problem, just don't continue in
      // this directory
      fprintf(stderr, "Error: `%s`: %s\n", l, strerror(errno));
      return 0;
    }
    fprintf(stderr,
            "Error while attempting to read directory \"%s\". err = %s\n", l,
            strerror(errno));
    // Otherwise, error out
    return 1;
  }
  if (l[strlen(l) - 1] == '/') {
    l[strlen(l) - 1] = '\0';
  }
  struct dirent *dire;
  while ((dire = readdir(d)) != NULL) {
    if ((long)dire == -1) {  // There was an error in readdir
      fprintf(
          stderr,
          "Error attempting to read next file in directory \"%s\". err = %s\n",
          l, strerror(errno));
      return 1;
    }
    int r;
    char path[PATH_MAX + 1];
    sprintf(path, "%s/%s", l, dire->d_name);
    if (lstat(path, ls) != 0) {
      fprintf(stderr,
              "Ran into error trying to get information on \"%s\". err = %s\n",
              l, strerror(errno));
      return 1;
    }
    if (strcmp(dire->d_name, ".") == 0) {
      continue;
    }
    if (strcmp(dire->d_name, "..") == 0) {
      continue;
    }
    if (dire->d_type == DT_DIR) {
      if ((r = recurse(path, ls)) != 0) {
        return r;
      }
      continue;
    }
    if ((r = print(path, ls)) != 0) {
      return r;
    }
  }
  return 0;
}

int max(int val1, int val2) { return (val1 > val2) ? val1 : val2; }

// Filename escaping based off implementation in GNU Findutils
// https://git.savannah.gnu.org/cgit/findutils.git/tree/lib/listfile.c?id=5768a03ddfb5e18b1682e339d6cdd24ff721c510
void printFileName(char *name) {
  char c;
  while ((c = *name++) != '\0') {
    switch (c) {
      case '\\':
        printf("\\\\");
        break;

      case '\n':
        printf("\\n");
        break;

      case '\b':
        printf("\\b");
        break;

      case '\r':
        printf("\\r");
        break;

      case '\t':
        printf("\\t");
        break;

      case '\f':
        printf("\\f");
        break;

      case ' ':
        printf("\\ ");
        break;

      case '"':
        printf("\\\"");
        break;
      default:
        if (c > 040 && c < 0177) {
          printf("%c", c);
        } else {
          printf("\\%03o", (unsigned int)c);
        }
    }
  }
}

// Print padding based off of print padding in GNU Findutils
// https://git.savannah.gnu.org/cgit/findutils.git/tree/lib/listfile.c?id=5768a03ddfb5e18b1682e339d6cdd24ff721c510
void _print(unsigned int inodeNumber, unsigned int blksize, char *mode,
            unsigned int nlink, char *user, char *group, unsigned int size,
            char *mtime, char *name, char *linkTarget) {
  // Some output numbers are subtracted by 1 or 2 to account for spaces.
  inode_number_width = max(printf(" %*i ", inode_number_width, inodeNumber) - 2,
                           inode_number_width);
  block_size_width =
      max(printf("%*i ", block_size_width, blksize) - 1, block_size_width);
  printf("%s ", mode);
  nlink_width = max(printf("%*i ", nlink_width, nlink) - 1, nlink_width);
  owner_width = max(printf("%-*s ", owner_width, user) - 1, owner_width);
  group_width = max(printf("%-*s ", group_width, group) - 1, group_width);
  file_size_width =
      max(printf("%*i ", file_size_width, size) - 1, file_size_width);
  printf("%s ", mtime);
  printFileName(name);
  if (linkTarget != NULL) {
    printf(" -> ");
    printFileName(linkTarget);
  }
  printf("\n");
}

int print(char *name, struct stat *ls) {
  // Get mode
  char mode[MODE_LENGTH + 1];
  convertModeFlags(ls->st_mode, mode);

  unsigned int inodeNumber = ls->st_ino;
  // st_blocks gives number of 512 byte blocks allocated but `find -ls` counts
  // in 1024 byte blocks unless POSIXLY_CORRECT env variable is set. To
  // replicate the behavior without the environment variable, divide the number
  // of blocks by 1024/512 = 2;
  unsigned int blksize = ls->st_blocks / 2;
  unsigned int nlink = ls->st_nlink;

  // Get user
  char user[MAX_NAME_LENGTH];
  struct passwd *u = getpwuid(ls->st_uid);
  if (u != NULL) {
    strcpy(user, u->pw_name);
  } else {
    sprintf(user, "%i", ls->st_uid);
  }

  // Get group
  char group[MAX_NAME_LENGTH];
  struct group *g = getgrgid(ls->st_gid);
  if (g != NULL) {
    strcpy(group, g->gr_name);
  } else {
    sprintf(group, "%i", ls->st_gid);
  }

  unsigned int size = ls->st_size;

  // String with enough space to store the full string regardless of set locale
  char mtime[256];
  strftime(mtime, 256, "%b %e %H:%M", localtime(&ls->st_mtim.tv_sec));

  // If it is a symlink
  if ((ls->st_mode & S_IFMT) == S_IFLNK) {
    // Get link target
    char linkTarget[PATH_MAX + 1];
    ssize_t len = readlink(name, linkTarget, PATH_MAX);
    if (len < 0) {
      fprintf(stderr, "Ran into error trying to read symlink `%s`. err = %s\n",
              name, strerror(errno));
      return 1;
    }
    // `readlink` does not null-terminate the string so we must put it ourselves
    linkTarget[len] = '\0';
    _print(inodeNumber, blksize, mode, nlink, user, group, size, mtime, name,
           linkTarget);
    return 0;
  }
  _print(inodeNumber, blksize, mode, nlink, user, group, size, mtime, name,
         NULL);
  return 0;
}

void setPermissions(unsigned int perm, char *s) {
  if ((perm & S_IROTH) == S_IROTH) {
    s[0] = 'r';
  }
  if ((perm & S_IWOTH) == S_IWOTH) {
    s[1] = 'w';
  }
  if ((perm & S_IXOTH) == S_IXOTH) {
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
    case S_IFREG:
      break;
    default:
      s[0] = '?';
      break;
  }

  // Set user permissions
  setPermissions(mode >> 6, s + 1);

  // Set group permissions
  setPermissions(mode >> 3, s + 4);

  // Set other permissions
  setPermissions(mode, s + 7);

  // Setting for setuid, setgid, and sticky
  if ((mode & S_ISGID) == S_ISGID) {
    if (s[6] == '-') {
      s[6] = 'S';
    } else {
      s[6] = 's';
    }
  }

  if ((mode & S_ISGID) == S_ISGID) {
    if (s[3] == '-') {
      s[3] = 'S';
    } else {
      s[3] = 's';
    }
  }

  if ((mode & S_ISVTX) == S_ISVTX) {
    if (s[9] == '-') {
      s[9] = 'T';
    } else {
      s[9] = 't';
    }
  }
}
