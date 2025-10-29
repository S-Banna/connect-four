CC = gcc
CFLAGS = -Wall -Werror
TARGET = app
SRC = main.c checkWin.c easyBot.c mediumBot.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

.PHONY: clean
clean:
	rm -f $(TARGET).exe