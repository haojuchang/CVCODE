CXX       := g++
CXXFLAGS  := -std=c++11 -O3 -Wall -Wextra -MMD
LIBS      := -L -lG -lm
INCLUDE   := -isystem ./src 
EXEC      := Lab1
SRC_DIRS  := .\
			 src/Parser \
			 src/Bin \

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

caseall: 
	@echo -n "Case 1, "
	@./$(EXEC) ./testcase/case1 ./result/output1.txt 300 700
	@echo -n "Case 2, "
	@./$(EXEC) ./testcase/case2 ./result/output2.txt 800 300
	@echo -n "Case 3, "
	@./$(EXEC) ./testcase/case3 ./result/output3.txt 1900 1000
	@echo -n "Case A, "
	@./$(EXEC) ./testcase/caseA ./result/outputA.txt 1400 700
	@echo -n "Case 4, "
	@./$(EXEC) ./testcase/case4 ./result/output4.txt 400 200
	@echo -n "Case 5, "
	@./$(EXEC) ./testcase/case5 ./result/output5.txt 1800 700
	@echo -n "Case 6, "
	@./$(EXEC) ./testcase/case6 ./result/output6.txt 1500 900
	@echo -n "Case B, "
	@./$(EXEC) ./testcase/caseB ./result/outputB.txt 1600 1000
	
case1: $(EXEC)
	./$(EXEC) ./testcase/case1 ./result/output1.txt
case2: $(EXEC)
	./$(EXEC) ./testcase/case2 ./result/output2.txt
case3: $(EXEC)
	./$(EXEC) ./testcase/case3 ./result/output3.txt
caseA: $(EXEC)
	./$(EXEC) ./testcase/caseA ./result/outputA.txt
case4: $(EXEC)
	./$(EXEC) ./testcase/case4 ./result/output4.txt
case5: $(EXEC)
	./$(EXEC) ./testcase/case5 ./result/output5.txt
case6: $(EXEC)
	./$(EXEC) ./testcase/case6 ./result/output6.txt
caseB: $(EXEC)
	./$(EXEC) ./testcase/caseB ./result/outputB.txt