CXX = g++
CXXFLAGS = -std=c++11 `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

# Target = nameOfExecuitable
TARGET = DetectHandContours

SRCS = main.cpp

# all : The default target that builds TARGET executable
all: $(TARGET) 

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)


clean:
	rm -f $(TARGET)