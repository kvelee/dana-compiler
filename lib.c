#include <stdio.h>
#include <string.h>

// --- Συναρτήσεις Εξόδου ---
void writeInteger(int n) { 
    printf("%d", n); 
}

void writeByte(char b) { 
    printf("%u", (unsigned char)b); 
}

void writeChar(char b) { 
    printf("%c", b); 
}

void writeString(char *s) { 
    printf("%s", s); 
}

// --- Συναρτήσεις Εισόδου ---
int readInteger() { 
    int n; 
    if (scanf("%d", &n) != 1) return 0; 
    return n; 
}

char readByte() { 
    unsigned int n; 
    if (scanf("%u", &n) != 1) return 0; 
    return (char)(n & 0xFF); 
}

char readChar() { 
    char c; 
    if (scanf(" %c", &c) != 1) return 0; 
    return c; 
}

void readString(int n, char *s) {
    int i = 0;
    int c = getchar();
    
    while (c == '\n' || c == '\r') c = getchar();
    
    while (c != '\n' && c != '\r' && c != EOF && i < n - 1) {
        s[i++] = (char)c;
        c = getchar();
    }
    s[i] = '\0';
}

// --- Συναρτήσεις Μετατροπής ---
int extend(char b) { 
    return (int)(unsigned char)b; 
}

// shrink: Κρατάμε μόνο τα 8 LSBs του int
char shrink(int i) { 
    return (char)(i & 0xFF); 
}

// --- Διαχείριση Συμβολοσειρών ---
int strlen_dana(char *s) {
    return (int)strlen(s);
}

int strcmp_dana(char *s1, char *s2) {
    return strcmp(s1, s2);
}

void strcpy_dana(char *dst, char *src) {
    strcpy(dst, src);
}

void strcat_dana(char *dst, char *src) {
    strcat(dst, src);
}