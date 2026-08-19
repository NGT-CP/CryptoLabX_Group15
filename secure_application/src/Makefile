CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET   = drone_system
SRCS     = main.cpp auth.cpp waypoint.cpp mission.cpp telemetry.cpp logger.cpp
OBJS     = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.txt

.PHONY: all clean
