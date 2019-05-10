export CC  = gcc
export CXX = g++-6
export CFLAGS = -std=c++11 -Wall -O3 -msse2  -fopenmp  -I..

BIN = ./bin/produce_candidate_patterns ./bin/rebuild_features ./bin/select_patterns ./bin/predict ./bin/combine_global_and_local
.PHONY: clean all

all: ./bin $(BIN)

./bin/produce_candidate_patterns: src/patterns/produce_candidate_patterns.cpp src/classification/*.h src/patterns/*.h src/utils/*.h
./bin/select_patterns: src/patterns/select_patterns.cpp src/patterns/*.h src/utils/*.h
./bin/rebuild_features: src/classification/rebuild_features.cpp src/classification/*.h src/patterns/*.h src/utils/*.h
./bin/combine_global_and_local: src/classification/combine_global_and_local.cpp src/classification/*.h src/patterns/*.h src/utils/*.h
./bin/predict: src/classification/predict.cpp src/patterns/*.h src/utils/*.h

./bin:
	mkdir bin

export LDFLAGS= -pthread -lm -Wno-unused-result -Wno-sign-compare -Wno-unused-variable -Wno-parentheses -Wno-format

$(BIN) :
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.cpp %.o %.c, $^)
$(OBJ) :
	$(CXX) -c $(CFLAGS) -o $@ $(firstword $(filter %.cpp %.c, $^) )

clean :
	rm -rf bin
