CC = gcc
CFLAGS = -Wall -Werror -pthread
TARGET = app
SRC = main.c checkWin.c easyBot.c mediumBot.c hardBot.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

.PHONY: clean
clean:
	rm -f $(TARGET).exe