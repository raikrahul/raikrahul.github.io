#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void garbage_stack() {
    volatile char buffer[128];
    memset((void*)buffer, 0x55, sizeof(buffer));
}

/*
 * Returns a string containing the last N characters of str,
 * or NULL if the length of str is not a multiple of N.
 */
char *lastN(const char *str, int n) {
    int len = strlen(str);
    if (len % n != 0) {
        return NULL;
    }
    char result[n + 1];
    strcpy(result, str + (len - n));
    char * volatile ptr = result;
    return ptr;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <string> [n]\n", argv[0]);
        return 1;
    }
    int n = 4;
    if (argc >= 3) {
        n = atoi(argv[2]);
    }
    char *res = lastN(argv[1], n);
    
    garbage_stack();

    if (res) {
        printf("Last %d characters: %s\n", n, res);
    } else {
        printf("Length is not a multiple of %d\n", n);
    }
    return 0;
}
