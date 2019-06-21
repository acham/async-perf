CXX = g++
CXXFLAGS = -Wall -std=c++14

main:
	$(CXX) $(CXXFLAGS) -o async-perf-cpp src/cpp/async_performance_test.cpp
	go build -o async-perf-go src/go/main.go

clean:
	rm async-perf-*
