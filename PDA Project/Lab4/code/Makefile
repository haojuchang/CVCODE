CXX       := g++
CXXFLAGS  := -std=c++11 -pthread -O3 -Wall -Wextra -MMD
LIBS      := -L -lG -lm -pthread 
INCLUDE   := -isystem ./src 
EXEC      := Lab4
SRC_DIRS  := .\
			 src \

SRCS      := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS      := $(SRCS:.cpp=.o)
DEPS      := $(OBJS:.o=.d)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -rf $(EXEC) $(OBJS) $(DEPS) result/*

test0: $(EXEC)
	./Lab4 ../Case/case0.txt result/output0.txt
	./../verifier result/output0.txt ../Case/case0.txt
test1: $(EXEC)
	./Lab4 ../Case/case1.txt result/output1.txt
	./../verifier result/output1.txt ../Case/case1.txt
test2: $(EXEC)
	./Lab4 ../Case/case2.txt result/output2.txt
	./../verifier result/output2.txt ../Case/case2.txt
test3: $(EXEC)
	./Lab4 ../Case/case3.txt result/output3.txt
	./../verifier result/output3.txt ../Case/case3.txt
testd: $(EXEC)
	./Lab4 ../Case/Deutsch_difficult.txt result/outputd.txt
	./../verifier result/outputd.txt ../Case/Deutsch_difficult.txt
