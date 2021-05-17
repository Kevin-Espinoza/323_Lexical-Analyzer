CC=g++
CFLAGS=-I.
DEPS = Lexer.h Parser.h Tokens.h State.h
OBJ = main.o Lexer.o Parser.o
ODIR=obj

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

CompilerProject: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)