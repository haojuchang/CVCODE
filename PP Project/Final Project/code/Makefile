c=0
max_thread=4
input_size=6 #as 2's exponent

TARGET = pp
CC = g++
CFLAGS =-std=c++11 -O3 -Wall -fopenmp

SOURCE_PATH = ./src
BINARY_PATH = ./bin

SRCS = $(wildcard $(SOURCE_PATH)/*.cpp)
OBJS = $(SRCS:%.cpp=%.o)
DEPS = $(SRCS:%.cpp=%.d)

all: $(OBJS)
	$(CC) $(CFLAGS) -I $(SOURCE_PATH) $^ -o $(BINARY_PATH)/$(TARGET)


-include $(DEPS)

%.o : %.cpp
	$(CC) $(CFLAGS) -I $(SOURCE_PATH) -MMD -c $< -o $@


case:
	$(BINARY_PATH)/$(TARGET) ./cases/case$(c)/io_info.txt ./cases/case$(c)/iogen ./cases/case$(c)/circuit.v $(max_thread) $(input_size)

clean:
	rm -f $(BINARY_PATH)/$(TARGET) $(OBJS) $(DEPS)

