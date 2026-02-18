#include <stdio.h>

// --- Συναρτήσεις Εξόδου ---
void writeInteger(short n) { printf("%d", n); }
void writeByte(char b) { printf("%d", b); }
void writeChar(char b) { printf("%c", b); }
void writeString(char *s) { printf("%s", s); }

// --- Συναρτήσεις Εισόδου ---
short readInteger() { short n; scanf("%hd", &n); return n; }
char readByte() { short n; scanf("%hd", &n); return (char)n; }
char readChar() { char c; scanf(" %c", &c); return c; }

void readString(short n, char *s) {
    int i = 0;
    char c = getchar();
    // Προσπέρασε τυχόν κενά/newlines που έμειναν στο buffer
    while(c == '\n' || c == ' ') c = getchar(); 
    // Διάβασε μέχρι το τέλος της γραμμής ή το όριο n
    while(c != '\n' && c != EOF && i < n - 1) {
        s[i++] = c;
        c = getchar();
    }
    s[i] = '\0';
}

// --- Συναρτήσεις Μετατροπής ---
short extend(char b) { return (short)b; }
char shrink(short i) { return (char)(i & 0xFF); }

// --- Συναρτήσεις Strings (Προσαρμοσμένες σε i16 αντί για size_t) ---
short strlen(char *s) {
    short len = 0;
    while(s[len]) len++;
    return len;
}

short strcmp(char *s1, char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return (short)(*s1 - *s2);
}

void strcpy(char *dst, char *src) {
    while((*dst++ = *src++));
}

void strcat(char *dst, char *src) {
    while(*dst) dst++;
    while((*dst++ = *src++));
}