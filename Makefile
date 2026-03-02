CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

TARGET = loadbalancer

SRCS = main.cpp LoadBalancer.cpp WebServer.cpp RequestQueue.cpp Request.cpp IPRange.cpp
HDRS = LoadBalancer.h WebServer.h RequestQueue.h Request.h IPRange.h
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) lb_log.txt

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run