/*
#  string functions for the null Kernel
#
##############################################################################
#
#  Copyright (C) 2023 Keegan Powers
#
#  This file is part of the null Kernel
#
#  The null Kernel is free software: you can redistribute it
#  and/or modify it under the terms of the GNU General Public
#  License as published by the Free Software Foundation, either
#  version 3 of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program. If not, see <https://www.gnu.org/licenses/>.
#
#############################################################################
*/

#include "string.h"

void strcatc(char *dst, char c) {
    unsigned short len = strlen(dst);

    dst[len++] = c;
    dst[len] = 0;

}

void strdelc(char *str) {
    unsigned short len;

    if (!*str) return;

    len = strlen(str);
    str[len - 1] = 0;

}

unsigned int hextoi(char *str) {
    unsigned int val = 0;
    unsigned char i;

    for (i = 0; str[i]; i++) {
        if (str[i] >= '0' && str[i] <= '9') str[i] = str[i] - '0';
        else if (str[i] >= 'a' && str[i] <= 'f') str[i] = str[i] - 'a' + 10;
        val = (val << 4) | (str[i] & 0xf);
    }
    
    return val;
        
}

char *itohex(unsigned int n) {
    unsigned char i = 7;
    unsigned char s = 0;
    unsigned int num = n;
    char buffer[8];

    while (num > 0) {
        buffer[i] = "0123456789abcdef"[num % 16];
        i--;
        num = num / 16;

    }

    while (++i < 8) {
        buffer[s++] = buffer[i];

    }

    buffer[s] = 0;

    return buffer;

}

char *strchr(char *s, int c) {
    while (*s != c) if (!*s++) return 0;
    return s;
}

int strcmp(char *s1, char *s2) {
    char *p1 = s1;
    char *p2 = s2;

    while (*p1 && (*p1 == *p2)) p1++, p2++;

    return *p1 - *p2;
}

unsigned short strlen(char *string) {
    unsigned short len;

    for (len = 0; string[len] != 0; len++);
    return len;
}

void strcpy(char *dst, char *src) {
    unsigned short i;

    for (i = 0; src[i] != 0; i++) dst[i] = src[i];
}

void strcat(char *dst, char *src) {
    unsigned short len = strlen(dst);

    unsigned short i;

    for (i = 0; src[i] != 0; i++) dst[len + i] = src[i];
}

static char *Last = 0;
char *strtok(char *s, char *delim) {

    char c;
    char* start;

    /* Use the stored location if called with a NULL pointer */
    if (s == 0) {
        s = Last;
    }

    /* If s is empty, there are no more tokens. Return 0 in this case. */
    if (*s == '\0') {
        return 0;
    }

    /* Search the address of the first element in s that equals none
    ** of the characters in delim.
    */
    while ((c = *s) && strchr(delim, c) != 0) {
        ++s;
    }
    if (c == '\0') {
        /* No more tokens found */
        Last = s;
        return 0;
    }

    /* Remember the start of the token */
    start = s;

    /* Search for the end of the token */
    while ((c = *s) && strchr (delim, c) == 0) {
        ++s;
    }
    if (c == '\0') {
        /* Last element */
        Last = s;
    } else {
        *s = '\0';
        Last = s + 1;
    }

    /* Return the start of the token */
    return start;
}

char *strupr(char *s) {
    char *sp = s;

    for (; *sp; sp++) {
        *sp = toupper(*sp);

    }

    return s;

}

char *strlwr(char *s) {
    char *sp = s;

    for (; *sp; sp++) {
        *sp = tolower(*sp);

    }

    return s;

}

char toupper(char c) {
    return c + (('A' - 'a') * (c >= 'a' && c <= 'z'));
}

char tolower(char c) {
    return c + (('a' - 'A') * (c >= 'A' && c <= 'Z'));
}

void reverse(char *s, int length) {
    int start = 0;
    int end = length - 1;

    while (start < end) {
        char temp = s[start];
        s[start] = s[end];
        s[end] = temp;
        end--;
        start++;

    }

}

char *itoa(long int n, char *buffer, int radix) {
    int i = 0;
    int isNegative = 0;

    if (n == 0) {
        buffer[i++] = '0';
        buffer[i] = 0;
        return buffer;

    }
 
    if (n < 0 && radix == 10) {
        isNegative = 1;
        n = -n;

    }
 
    while (n != 0) {
        int rem = n % radix;
        buffer[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        n = n / radix;

    }

    if (isNegative) buffer[i++] = '-';
 
    buffer[i] = 0;
 
    reverse(buffer, i);
 
    return buffer;

}