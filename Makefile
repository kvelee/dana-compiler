.PHONY: clean distclean default

CXX=g++

default: danac

lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

lexer.o: lexer.cpp lexer.hpp parser.hpp ast.hpp

parser.hpp parser.cpp: parser.y ast.hpp
	bison -dv -o parser.cpp parser.y -Wcounterexamples
parser.o: parser.cpp lexer.hpp

danac: lexer.o parser.o
	$(CXX) -o danac lexer.o parser.o

clean:
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o

distclean: clean
	$(RM) danac