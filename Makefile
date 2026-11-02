
CXXFLAGS += -std=c++23

all: process

.SUFFIXES = .png .csv

process_benchmark: process_benchmark.o
	$(CXX) $(CXXFLAGS) $^ -o $@

process_benchmark.png: process_benchmark.csv
	python benchmark_process.py $<

process_benchmark.csv:
	./process_benchmark

process: process_benchmark process_benchmark \
					process_benchmark.png

CLEANFILES = process_benchmark_cpp \
						 process_benchmark.csv \
						 process_benchmark.png

clean:
	rm -rf *.o $(CLEANFILES)
