CC=gcc
CFLAGS=-g
DEPS = symbolList.h bool.h
OBJS = resolve.o symbolList.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

resolve: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

symbolListTest: symbolListTest.o symbolList.o
	$(CC) -o symbolListTest symbolList.o symbolListTest.o $(CFLAGS)

clean:
	rm -f resolve
	rm -f symbolListTest
	rm -r -f ./*.o