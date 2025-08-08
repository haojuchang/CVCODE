CXX       := g++
CXXFLAGS  := -std=c++11 -O3 -Wall -Wextra -MMD
LIBS      := -L -lG -lm
INCLUDE   := -isystem ./src 
EXEC      := Lab1
SRC_DIRS  := .\
			 src/CornerStitching \
			 src/Parser \

SRCS      := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS      := $(SRCS:.cpp=.o)
DEPS      := $(OBJS:.o=.d)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -rf $(EXEC) $(OBJS) $(DEPS) result/*.txt result/*.png

test0: $(EXEC)
	./$(EXEC) ../testcase/case0.txt ./result/output0.txt -L ./result/layout0.txt
	# python3 draw_block_layout.py ./result/layout0.txt ./result/layout0.png
test1: $(EXEC)
	./$(EXEC) ../testcase/case1.txt ./result/output1.txt -L ./result/layout1.txt
	# python3 draw_block_layout.py ./result/layout1.txt ./result/layout1.png
test2: $(EXEC)
	./$(EXEC) ../testcase/case2.txt ./result/output2.txt -L ./result/layout2.txt
	# python3 draw_block_layout.py ./result/layout2.txt ./result/layout2.png
test7: $(EXEC)
	./$(EXEC) ../testcase/case7.txt ./result/output7.txt -L ./result/layout7.txt
	# python3 draw_block_layout.py ./result/layout7.txt ./result/layout7.png
testN0: $(EXEC)
	./$(EXEC) ../testcase/caseN0.txt ./result/outputN0.txt -L ./result/layoutN0.txt
	#python3 draw_block_layout.py ./result/layoutN0.txt ./result/layoutN0.png
testN1: $(EXEC)
	./$(EXEC) ../testcase/caseN1.txt ./result/outputN1.txt -L ./result/layoutN1.txt
	#python3 draw_block_layout.py ./result/layoutN1.txt ./result/layoutN1.png
testN2: $(EXEC)
	./$(EXEC) ../testcase/caseN2.txt ./result/outputN2.txt -L ./result/layoutN2.txt
	#python3 draw_block_layout.py ./result/layoutN2.txt ./result/layoutN2.png
testN3: $(EXEC)
	./$(EXEC) ../testcase/caseN3.txt ./result/outputN3.txt -L ./result/layoutN3.txt
	#python3 draw_block_layout.py ./result/layoutN3.txt ./result/layoutN3.png
testN4: $(EXEC)
	time ./$(EXEC) ../testcase/caseN4.txt ./result/outputN4.txt -L ./result/layoutN4.txt
	# python3 draw_block_layout.py ./result/layoutN4.txt ./result/layoutN4.png

testall: test0 test1 test2 test7

diffall:
	diff ../output_ans/output0.txt result/output0.txt
	diff ../output_ans/output1.txt result/output1.txt
	diff ../output_ans/output2.txt result/output2.txt
	diff ../output_ans/output7.txt result/output7.txt

.PHONY: all clean test
-include $(DEPS)
