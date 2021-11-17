#include <stdio.h>

int getBit(char* str, int pos) {
    pos = 31 - pos;
    unsigned char l = 8;
    l = l >> (pos % 4);
    unsigned char a = str[pos / 4];
    if (a >= '0' && a <= '9') {
        a = a - '0';
    } else {
        a = a - 'a' + 10;
    }
    return (a & l) == l;
}


int main(int argc, char** argv) {
    char* str = argv[1];
    // char* str = "20000010";
    // char* s = "00000000";
    // for (int i = 0; i < 8; i++) {
    //     s[i] = str[7 - i];
    // }

    char dirty = getBit(str, 26)? 'D': '-';
    char accessed = getBit(str, 25)? 'A': '-';
    char read = getBit(str, 29) ? 'R': '-';
    char write = getBit(str, 28) ? 'W': '-';
    char execute = getBit(str, 27) ? 'X': '-';
    char present = getBit(str, 31)? 'P': '-';
    char supervisor = getBit(str, 30)? 'S':'-';
    printf("%c%c%c%c%c%c%c\n", present, supervisor, read, write, execute, dirty, accessed);
}

