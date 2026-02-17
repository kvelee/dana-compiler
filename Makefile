.PHONY: clean distclean default

LLVM_CONFIG=llvm-config-18

CXX=g++ `llvm-config-18 --cxxflags`
CXXFLAGS=-Wall -g -std=c++17 `$(LLVM_CONFIG) --cxxflags`
LDFLAGS=`$(LLVM_CONFIG) --ldflags --system-libs --libs core`

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
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o *.ll

distclean: clean
	$(RM) danac