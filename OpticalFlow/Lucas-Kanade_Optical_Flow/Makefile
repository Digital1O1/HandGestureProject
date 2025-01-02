CXX = g++
CXXFLAGS = -std=c++11 -g `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`
TARGET = OpticalFlow_HandTracker
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

