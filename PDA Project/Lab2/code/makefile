CXX       := g++
CXXFLAGS  := -std=c++11 -O3 -Wall -Wextra -MMD
LIBS      := -L -lG -lm
INCLUDE   := -isystem ./src 
EXEC      := Lab2
SRC_DIRS  := .\
			 src/Parser \
			 src/Bstar \
			#  src/Floorplan \

SRCS      := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS      := $(SRCS:.cpp=.o)
DEPS      := $(OBJS:.o=.d)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -rf $(EXEC) $(OBJS) $(DEPS) result/*.rpt result/*.HPWL result/*.png result/*.txt

# ================================================================================
verify = ../verifier
alpha = 0.5

test1: $(EXEC)
	./$(EXEC) $(alpha) ../testcase/ami33/ami33.block ../testcase/ami33/ami33.nets result/ami33_output.rpt
	python3 draw_block_layout.py ./result/vis.txt ./result/vis.png
	./../verifier $(alpha) ../testcase/ami33/ami33.block ../testcase/ami33/ami33.nets result/ami33_output.rpt
test2: $(EXEC)
	./$(EXEC) $(alpha) ../testcase/ami49/ami49.block ../testcase/ami49/ami49.nets result/ami49_output.rpt
	python3 draw_block_layout.py ./result/vis.txt ./result/vis.png
	./../verifier $(alpha) ../testcase/ami49/ami49.block ../testcase/ami49/ami49.nets result/ami49_output.rpt
test3: $(EXEC)
	./$(EXEC) $(alpha) ../testcase/vda317b/vda317b.block ../testcase/vda317b/vda317b.nets result/vda317b_output.rpt
	python3 draw_block_layout.py ./result/vis.txt ./result/vis.png
	./../verifier $(alpha) ../testcase/vda317b/vda317b.block ../testcase/vda317b/vda317b.nets result/vda317b_output.rpt
testall: test1 test2 test3


ver1:
	./$(verify) $(alpha) ../testcase/ami33/ami33.block ../testcase/ami33/ami33.nets result/ami33_output.rpt
ver2:
	./$(verify) $(alpha) ../testcase/ami49/ami49.block ../testcase/ami49/ami49.nets result/ami49_output.rpt
ver3:
	./$(verify) $(alpha) ../testcase/vda317b/vda317b.block ../testcase/vda317b/vda317b.nets result/vda317b_output.rpt
verall: ver1 ver2 ver3
