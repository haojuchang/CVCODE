CASEDIR = info/program2

all:
	g++ -O3 -std=c++11 main.cpp netlist.cpp parser.cpp -I . -o 410510026.o

test1:
	./410510026.o $(CASEDIR)/c17.v -p $(CASEDIR)/c17.pat -l $(CASEDIR)/test_lib.lib
	diff 410510026_c17_load.txt $(CASEDIR)/new_golden/golden_c17_load.txt
	diff 410510026_c17_delay.txt $(CASEDIR)/new_golden/golden_c17_delay.txt
	diff 410510026_c17_path.txt $(CASEDIR)/new_golden/golden_c17_path.txt
test2:
	./410510026.o $(CASEDIR)/c17md.v -p $(CASEDIR)/c17md.pat -l $(CASEDIR)/test_lib.lib
	diff 410510026_c17md_load.txt $(CASEDIR)/new_golden/golden_c17_load.txt
	diff 410510026_c17md_delay.txt $(CASEDIR)/new_golden/golden_c17_delay.txt
	diff 410510026_c17md_path.txt $(CASEDIR)/new_golden/golden_c17_path.txt
	
test3:
	./410510026.o $(CASEDIR)/c432.v -p $(CASEDIR)/c432.pat -l $(CASEDIR)/test_lib.lib
	diff 410510026_c432_load.txt $(CASEDIR)/new_golden/golden_c432_load.txt
	diff 410510026_c432_delay.txt $(CASEDIR)/new_golden/golden_c432_delay.txt
	diff 410510026_c432_path.txt $(CASEDIR)/new_golden/golden_c432_path.txt

clean:
	rm ./410510026.o
	rm *.txt