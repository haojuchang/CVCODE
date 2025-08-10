c=1

all:
	g++ -std=c++11 main.cpp -I . -o ppfinal

sample:
	./ppfinal ../cases/sample/io_info.txt ../cases/sample/iogen ../cases/sample/circuit.v
case:
	./ppfinal ../cases/case$(c)/io_info.txt ../cases/case$(c)/iogen ../cases/case$(c)/circuit.v

clean:
	rm -f core *~ $(TARGET); \
	rm *.o

