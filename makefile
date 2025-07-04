CC = gcc
CFLAGS = -Wall -Wextra -g

SRC_DIR = src
LIB_DIR = $(SRC_DIR)/lib
OUT = server

# Collect all .c files in src/ and src/lib/
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
LIB_FILES := $(wildcard $(LIB_DIR)/*.c)
SRCS := $(SRC_FILES) $(LIB_FILES)

# Generate object file names from .c files
OBJS := $(patsubst %.c,%.o,$(SRCS))

all: $(OUT)

$(OUT): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUT)

clean:
	rm -f $(OUT) $(OBJS)

.PHONY: all clean
