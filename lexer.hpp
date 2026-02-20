#include <string>

int yylex();
void yyerror(const char *msg);

std::string unescapeString(const std::string& input);