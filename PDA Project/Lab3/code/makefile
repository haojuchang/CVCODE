CXX       := g++
CXXFLAGS  := -std=c++11 -pthread -O3 -Wall -Wextra -MMD
LIBS      := -L -lG -lm -pthread 
INCLUDE   := -isystem ./src 
EXEC      := Lab3
SRC_DIRS  := .\
			 src/Parser \
			 src/Partition \
			 src/Legalize \

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

# ================================================================================

VERIFIER=../verifier
DRAWER  =../draw.py

CASE0=../testcase/case0.txt
CASE1=../testcase/case1.txt
CASE2=../testcase/case2.txt
CASE3=../testcase/case3.txt
CASEX=../testcase/casex.txt
CASE25=../testcase/case25.txt
OUTP0=./result/outp0.txt
OUTP1=./result/outp1.txt
OUTP2=./result/outp2.txt
OUTP3=./result/outp3.txt
OUTPX=./result/outpx.txt
OUTP25=./result/outp25.txt
DRAW0=./result/draw0
DRAW1=./result/draw1
DRAW2=./result/draw2
DRAW3=./result/draw3


test0: $(EXEC)
	time ./$(EXEC) $(CASE0) $(OUTP0)
	$(VERIFIER) $(CASE0) $(OUTP0)
	python3 $(DRAWER) $(CASE0) $(OUTP0) $(DRAW0)
test1: $(EXEC)
	time ./$(EXEC) $(CASE1) $(OUTP1)
	$(VERIFIER) $(CASE1) $(OUTP1)
	python3 $(DRAWER) $(CASE1) $(OUTP1) $(DRAW1)
test2: $(EXEC)
	time ./$(EXEC) $(CASE2) $(OUTP2)
	$(VERIFIER) $(CASE2) $(OUTP2)
	python3 $(DRAWER) $(CASE2) $(OUTP2) $(DRAW2)
test3: $(EXEC)
	time ./$(EXEC) $(CASE3) $(OUTP3)
	$(VERIFIER) $(CASE3) $(OUTP3)
	python3 $(DRAWER) $(CASE3) $(OUTP3) $(DRAW3)
testx: $(EXEC)
	time ./$(EXEC) $(CASEX) $(OUTPX)
	$(VERIFIER) $(CASEX) $(OUTPX)
test25: $(EXEC)
	time ./$(EXEC) $(CASE25) $(OUTP25)
	$(VERIFIER) $(CASE25) $(OUTP25)
testall: test0 test1 test2 test3 testx
