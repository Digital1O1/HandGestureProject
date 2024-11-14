CXX = g++
CXXFLAGS = -std=c++11 `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

TARGET = adaptiveThreshold

SRCS = main.cpp
# Speccifies what should happen when the user runs make
# Builds the target by calling the compiler and linking it with the specified flags
all: $(TARGET) 

# $(TARGET): $(SRCS) --> Specifies target rule for make file that tells the makefile '"When I'm run as the default rule (i.e., 'make' without specifying a target), and I've got all the necessary dependencies specified, then perform the actions in the next rule."




$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)


clean:
	rm -f $(TARGET)