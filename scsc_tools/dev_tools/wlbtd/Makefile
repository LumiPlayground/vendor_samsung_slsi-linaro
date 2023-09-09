# FILE
# Makefile  -  makefile for Linux WLBTd

# Source Files
SRC_C = wlbtd.cpp wlbtd_sable.cpp wlbtd.h

CC = g++
FLAGS = -Wall -g -pthread
LIB_PATH = /usr/lib/
INC_PATH =/usr/include/libnl3
LDFLAGS = -lpthread -lnl-genl-3 -lnl-3

# Target executable
TARGET = wlbtd

all: $(TARGET)

$(TARGET): $(SRC_C)
	$(CC) $(FLAGS) -L$(LIB_PATH) -I$(INC_PATH) $(SRC_C) $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf $(TARGET)

.PHONY: clean
