#ifndef __BUILT_INS_HPP__
#define __BUILT_INS_HPP__

#include "symbol.hpp"

/**
 * Γεμίζει το SymbolTable με τις προκαθορισμένες συναρτήσεις της Dana.
 * Χρησιμοποιεί το TYPE_FUNC ως return_value για τις διαδικασίες (void).
 */
inline void populate_builtins(SymbolTable &st) {

    // --- Συναρτήσεις Εξόδου (Procedures/Void)  ---
    st.insert("writeInteger", TYPE_FUNC, TYPE_FUNC, {TYPE_INT}, false, 0, {0});
    st.insert("writeByte",    TYPE_FUNC, TYPE_FUNC, {TYPE_BYTE}, false, 0, {0});
    st.insert("writeChar",    TYPE_FUNC, TYPE_FUNC, {TYPE_BYTE}, false, 0, {0});
    st.insert("writeString",  TYPE_FUNC, TYPE_FUNC, {TYPE_REF_BYTE}, false, 0, {1}); 

    // --- Συναρτήσεις Εισόδου  ---
    st.insert("readInteger",  TYPE_FUNC, TYPE_INT,  {}, false, 0, {});
    st.insert("readByte",     TYPE_FUNC, TYPE_BYTE, {}, false, 0, {});
    st.insert("readChar",     TYPE_FUNC, TYPE_BYTE, {}, false, 0, {});
    st.insert("readString",   TYPE_FUNC, TYPE_FUNC, {TYPE_INT, TYPE_REF_BYTE}, false, 0, {0, 1}); 
    
    // --- Συναρτήσεις Μετατροπής  ---
    st.insert("extend",       TYPE_FUNC, TYPE_INT,  {TYPE_BYTE}, false, 0, {0});
    st.insert("shrink",       TYPE_FUNC, TYPE_BYTE, {TYPE_INT}, false, 0, {0});

    // --- Συναρτήσεις Συμβολοσειρών  ---
    st.insert("strlen",       TYPE_FUNC, TYPE_INT,  {TYPE_REF_BYTE}, false, 0, {1});
    st.insert("strcmp",       TYPE_FUNC, TYPE_INT,  {TYPE_REF_BYTE, TYPE_REF_BYTE}, false, 0, {1, 1});
    st.insert("strcpy",       TYPE_FUNC, TYPE_FUNC, {TYPE_REF_BYTE, TYPE_REF_BYTE}, false, 0, {1, 1});
    st.insert("strcat",       TYPE_FUNC, TYPE_FUNC, {TYPE_REF_BYTE, TYPE_REF_BYTE}, false, 0, {1, 1});
}

#endif // __BUILT_INS_HPP__