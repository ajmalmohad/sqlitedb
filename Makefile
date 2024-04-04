CC=gcc
CFLAGS=-g
SOURCES=./src/constants.c ./src/node.c ./src/btree.c ./src/serialize.c ./src/pager.c ./src/cursor.c ./src/main.c
EXECUTABLE=main
DB_FILE=main.db

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE) $(DB_FILE)

clean:
	rm -f $(EXECUTABLE)
