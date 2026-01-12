
CC = g++
CFLAGS = -Wall -std=c++98
SRCS = src/main.cpp src/core.cpp src/ops.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = bin/vip_system

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

test: src/core.cpp src/ops.cpp tests/test_core.cpp
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/test_core src/core.cpp src/ops.cpp tests/test_core.cpp

clean:
	rm -f $(TARGET) bin/test_core

