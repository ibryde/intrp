all: main

main: basics.o operations.o tools.o parser.o main.o

.PHONY: clean

clean:
	$(RM) *.o
	$(RM) main0