all: intrp

intrp: syscall.o basics.o operations.o tools.o parser.o main.o
	gcc $^ -o intrp

.PHONY: clean

clean:
	$(RM) *.o
	$(RM) intrp
