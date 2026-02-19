#include <stdio.h>

// --- Συναρτήσεις Εξόδου ---
void writeInteger(int n) { printf("%d", n); }

void writeByte(char b) { printf("%d", (unsigned char)b); }

void writeChar(char b) { printf("%c", b); }
void writeString(char *s) { printf("%s", s); }


// --- Συναρτήσεις Εισόδου ---
int readInteger() { 
    int n; 
    scanf("%d", &n); 
    return n; 
}

char readByte() { 
    int n; 
    scanf("%d", &n); 
    return (char)n; 
}

char readChar() { 
    char c; 
    scanf(" %c", &c); 
    return c; 
}

void readString(int n, char *s) {
    int i = 0;
    char c = getchar();
    while(c == '\n' || c == ' ') c = getchar(); 
    
    while(c != '\n' && c != EOF && i < n - 1) {
        s[i++] = c;
        c = getchar();
    }
    s[i] = '\0';
}


// --- Συναρτήσεις Μετατροπής ---
int extend(char b) { return (int)(unsigned char)b; }

// Κρατάμε μόνο τα 8 χαμηλότερα bits του int
char shrink(int i) { return (char)(i & 0xFF); }


int strlen(char *s) {
    int len = 0;
    while(s[len]) len++;
    return len;
}

int strcmp(char *s1, char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return (int)(*s1 - *s2);
}

void strcpy(char *dst, char *src) {
    while((*dst++ = *src++));
}

void strcat(char *dst, char *src) {
    while(*dst) dst++;
    while((*dst++ = *src++));
}