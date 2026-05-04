# Host-side test runner for game3 helpers.
# Usage:
#   make            build and run the suite
#   make build      compile only
#   make clean      remove the binary
#
# The -DGAME3_HOST_TESTS flag flips on the host-only block in
# test_game3_helpers.c. Without that define the file is empty, which
# is what the STM32 firmware build sees so there's no main() collision.

CC      = gcc
CFLAGS  = -Wall -Wextra -O0 -g -DGAME3_HOST_TESTS
TARGET  = test_game3
SRC     = test_game3_helpers.c

.PHONY: all build run clean

all: run

build: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
