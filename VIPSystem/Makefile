CXX = g++
CXXFLAGS = -std=c++98 -Wall -O2 -Iinclude
SRCS = src/main.cpp src/core.cpp src/user.cpp src/product.cpp src/transaction.cpp src/db_os.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = vipsystem

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
