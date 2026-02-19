.PHONY: clean distclean default

LLVM_CONFIG=llvm-config-18

CXX=g++
CXXFLAGS=-Wall -Wno-unused-variable -g -std=c++17 `$(LLVM_CONFIG) --cxxflags`
LDFLAGS=`$(LLVM_CONFIG) --ldflags --system-libs --libs core passes`

default: danac

lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

lexer.o: lexer.cpp lexer.hpp parser.hpp ast.hpp symbol.hpp

parser.hpp parser.cpp: parser.y ast.hpp symbol.hpp
	bison -dv -o parser.cpp parser.y

parser.o: parser.cpp lexer.hpp

codegen.o: codegen.cpp ast.hpp symbol.hpp
	$(CXX) $(CXXFLAGS) -c codegen.cpp

danac: lexer.o parser.o codegen.o
	$(CXX) $(CXXFLAGS) -o danac lexer.o parser.o codegen.o $(LDFLAGS)

clean:
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o *.ll *.exe *.output *.imm *.asm a.out

distclean: clean
	$(RM) danac