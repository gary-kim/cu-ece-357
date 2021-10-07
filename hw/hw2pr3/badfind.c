#include <dirent.h>
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
#include <errno.h>

const unsigned int MODE_LENGTH = 10;
const unsigned int SIZE_UNITS = 1 << 10;
const unsigned int MAX_NAME_LENGTH = 1 << 5;

int print(char *name, struct stat *ls);
int recurse(char *l, struct stat *ls);
void formatOutput(unsigned int inodeNumber, unsigned int size1k, char mode[], unsigned int nlink, char user[], char group[], unsigned int size, char mtime[], char *name, char linkTarget[], int option);
// char* utos(unsigned int input);
void convertModeFlags(unsigned int mode, char *s);

int main(int argc, char **argv) {
  char *l = ".";
  if (argc != 1) {
    l = argv[1];
  }
  struct stat ls;
  if (lstat(l, &ls) != 0) {
    fprintf(stderr, "Ran into error trying to get information on \"%s\". err = %s", l, strerror(errno));
    return 0;
  }
  return recurse(l, &ls);
}

int recurse(char *l, struct stat *ls) {
  print(l, ls);
  DIR *d = opendir(l);
  if (d == NULL) {
    if (errno == EACCES) {
      // If the problem is simply a permissions problem, just don't continue in this directory
      return 0;
    }
    fprintf(stderr, "Error while attempting to read directory \"%s\". err = %s", l, strerror(errno));
    // Otherwise, error out
    return 1;
  }
  if (l[strlen(l) - 1] == '/') {
    l[strlen(l) - 1] = '\0';
  }
  struct dirent *dire;
  while ((dire = readdir(d)) != NULL) {
    if ((long) dire == -1) { // There was an error in readdir
      fprintf(stderr, "Error attempting to read next file in directory \"%s\". err = %s", l, strerror(errno));
      return 1;
    }
    int r;
    char path[PATH_MAX];
    sprintf(path, "%s/%s", l, dire->d_name);
    if (lstat(path, ls) != 0) {
      fprintf(stderr, "Ran into error trying to get information on \"%s\". err = %s", l, strerror(errno));
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
    if((r = print(path, ls)) != 0) {
      return r;
    }
  }
  return 0;
}

int print(char *name, struct stat *ls) {
  // Get mode
  char mode[MODE_LENGTH + 1];
  convertModeFlags(ls->st_mode, mode);

  unsigned int inodeNumber = ls->st_ino;
  unsigned int size1k = ls->st_size / SIZE_UNITS;
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
  strftime(mtime, 256, "%b %2d %H:%M", localtime(&ls->st_mtim.tv_sec));

  // If it is a symlink
  if ((ls->st_mode & S_IFMT) == S_IFLNK) {
    // Get link target
    char linkTarget[PATH_MAX];

    ssize_t len = readlink(name, linkTarget, PATH_MAX);
    if (len < 0) {
      fprintf(stderr, "Ran into error trying to read symlink. err = %s\n", strerror(errno));
      return 1;
    }
    // `readlink` does not null-terminate the string so we must put it ourselves
    linkTarget[len] = '\0';
    formatOutput(inodeNumber, size1k, mode, nlink, user, group, size, mtime, name, linkTarget, 0);
  } else {
    char linkTarget[PATH_MAX];
    formatOutput(inodeNumber, size1k, mode, nlink, user, group, size, mtime, name, linkTarget, 1);
  }
  return 0;
}

void formatOutput(unsigned int inodeNumber, unsigned int size1k, char mode[], unsigned int nlink, char user[], char group[], unsigned int size, char mtime[], char *name, char linkTarget[], int option) {
  int base = 10;
  char buffer[20];
  char *size1kS = utoa(size1k, buffer, base);
  char *nlinkS = utoa(nlink, buffer, base);
  char *sizeS = utoa(size, buffer, base);
  char size1kPadding[4 - strlen(size1kS)];
  size1kS = strcat(strset(size1kPadding,' '),size1kS);
  char nlinkPadding[3 - strlen(*nlinkS)];
  nlinkS = strcat(strset(nlinkPadding,' '),nlinkS);
  char sizePadding[10 - strlen(*sizeS)];
  sizeS = strcat(strset(sizePadding, ' '),sizeS);

  if(option == 0){
    printf("%i %i %s %i %s %s %i %s %s -> %s\n", inodeNumber, size1k, mode,
           nlink, user, group, size, mtime, name, linkTarget);
  }else{
    printf("%i %i %s %i %s %s %i %s %s\n", inodeNumber, size1k, mode, nlink,
           user, group, size, mtime, name);
  }
}

// char* utos(unsigned int input) {

//   return 
// }

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
}
