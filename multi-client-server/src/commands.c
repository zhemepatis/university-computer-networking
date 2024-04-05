#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "commands.h"

#define BUFF_LEN 256

char *parseNext(char *str) {
    regex_t regex;
    regmatch_t match;
    const char *pattern = "[:space:]*(#?[^[:space:]]+)[:space:]*";
    
    char *result = NULL;
    int start;
    int end;
    int len;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        return NULL;
    }

    if (regexec(&regex, str, 1, &match, 0) == 0) {
        start = match.rm_so;
        end = match.rm_eo;
        len = end - start;

        result = calloc(len + 1, sizeof(char));
        strncpy(result, str + start, len);

        memmove(str, str + end, strlen(str) - len + 1);
    }

    // cleanup
    regfree(&regex);
    return result;
}