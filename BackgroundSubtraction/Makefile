CXX = g++
CXXFLAGS = -std=c++11 `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

TARGET = backgroundSubtraction
TARGET2 = matrixBSTesting

SRCS = main.cpp
SRCS2 = matrixTest.cpp

all: $(TARGET) $(TARGET2)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

$(TARGET2): $(SRCS2)
	$(CXX) $(CXXFLAGS) -o $(TARGET2) $(SRCS2) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(TARGET2)
