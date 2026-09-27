CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = pwlgen
PREFIX = /usr/local

$(TARGET): main.o
	$(CC) -o $(TARGET) main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

install: $(TARGET)
	install -m 755 $(TARGET) $(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET) main.o