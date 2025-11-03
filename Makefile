.PHONY: clean distclean default

CXX=g++

default: danac

lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

lexer.o: lexer.cpp lexer.hpp parser.hpp ast.hpp symbol.hpp

parser.hpp parser.cpp: parser.y ast.hpp symbol.hpp
	bison -dv -o parser.cpp parser.y
parser.o: parser.cpp lexer.hpp

danac: lexer.o parser.o
	$(CXX) -o danac lexer.o parser.o

test:
	@echo "Running tests on first 5 .dana files in ../Dana/ directory..."
	@i=1; \
	for file in ../Dana/*.dana; do \
		if [ -f "$$file" ]; then \
				echo "[$$i] Testing: $$file"; \
				./danac < "$$file"; \
				i=$$((i+1));\
				echo "===============================";\
			else \
				break; \
			fi; \
	done

clean:
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o

distclean: clean
	$(RM) danac