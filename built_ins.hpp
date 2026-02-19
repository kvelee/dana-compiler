#ifndef __BUILT_INS_HPP__
#define __BUILT_INS_HPP__

#include "symbol.hpp"

/**
 * Γεμίζει το SymbolTable με τις προκαθορισμένες συναρτήσεις της Dana.
 * Χρησιμοποιεί το TYPE_FUNC ως return_value για τις διαδικασίες (void).
 */
inline void populate_builtins(SymbolTable &st) {

    // --- Συναρτήσεις Εξόδου (Procedures/Void) [cite: 392-398] ---
    st.insert("writeInteger", TYPE_FUNC, TYPE_FUNC);
    st.insert("writeByte",    TYPE_FUNC, TYPE_FUNC);
    st.insert("writeChar",    TYPE_FUNC, TYPE_FUNC);
    st.insert("writeString",  TYPE_FUNC, TYPE_FUNC);

    // --- Συναρτήσεις Εισόδου [cite: 401-404] ---
    st.insert("readInteger",  TYPE_FUNC, TYPE_INT);
    st.insert("readByte",     TYPE_FUNC, TYPE_BYTE);
    st.insert("readChar",     TYPE_FUNC, TYPE_BYTE);
    st.insert("readString",   TYPE_FUNC, TYPE_FUNC); // Void procedure

    // --- Συναρτήσεις Μετατροπής [cite: 410-411] ---
    st.insert("extend",       TYPE_FUNC, TYPE_INT);
    st.insert("shrink",       TYPE_FUNC, TYPE_BYTE);

    // --- Συναρτήσεις Συμβολοσειρών [cite: 415-418] ---
    st.insert("strlen",       TYPE_FUNC, TYPE_INT);
    st.insert("strcmp",       TYPE_FUNC, TYPE_INT);
    st.insert("strcpy",       TYPE_FUNC, TYPE_FUNC); // Void procedure
    st.insert("strcat",       TYPE_FUNC, TYPE_FUNC); // Void procedure
}

#endif // __BUILT_INS_HPP__