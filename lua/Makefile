CC=gcc

all:lua

clean:
	rm lua *.o

lua: lua.o
	$(CC) -o $@ $< -llua

%.o: %.c
	$(CC) -c $< -o $@
