CXX       := g++
CXXFLAGS  := -std=c++11 -O3 -Wall -Wextra -MMD
LIBS      := -L -lG -lm
INCLUDE   := -isystem ./src 
EXEC      := Lab2
SRC_DIRS  := .\
			src/Parser \
			 src/MBFF \

SRCS      := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS      := $(SRCS:.cpp=.o)
DEPS      := $(OBJS:.o=.d)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -rf $(EXEC) $(OBJS) $(DEPS) ./result/*

caseall: case1 case2 case3 case4 case5
	
case1: $(EXEC)
	@echo '==========================='
	time ./$(EXEC) ./testcase/c1.in ./result/c1.out
	./testcase/SolutionChecker ./testcase/c1.in ./result/c1.out 
	@echo
case2: $(EXEC)
	@echo '==========================='
	time ./$(EXEC) ./testcase/c2.in ./result/c2.out
	./testcase/SolutionChecker ./testcase/c2.in ./result/c2.out 
	@echo
case3: $(EXEC)
	@echo '==========================='
	time ./$(EXEC) ./testcase/c3.in ./result/c3.out
	./testcase/SolutionChecker ./testcase/c3.in ./result/c3.out 
	@echo
case4: $(EXEC)
	@echo '==========================='
	time ./$(EXEC) ./testcase/c4.in ./result/c4.out
	./testcase/SolutionChecker ./testcase/c4.in ./result/c4.out 
	@echo
case5: $(EXEC)
	@echo '==========================='
	time ./$(EXEC) ./testcase/c5.in ./result/c5.out
	./testcase/SolutionChecker ./testcase/c5.in ./result/c5.out 
	@echo