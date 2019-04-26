OBJS = \
	dump.o\
	elflib.o\
	elfpack.o\
	stub.o\

CC = gcc
CFLAGS = -Wall

binary_packer: main.o $(OBJS)
	$(CC) $(CFLAGS) -o binary_packer main.o $(OBJS)

clean:
	rm *.o

form:
	clang-format-6.0 -i *.[ch]
