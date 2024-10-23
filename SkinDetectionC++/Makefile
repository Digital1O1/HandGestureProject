CXX = g++
CXXFLAGS = -std=c++11 `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

TARGET = skinDetectionC++
TARGET2 = singleImageTest

all: $(TARGET) $(TARGET2)

$(TARGET): main.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o $(LDFLAGS)

$(TARGET2): singleImageTest.o
	$(CXX) $(CXXFLAGS) -o $(TARGET2) singleImageTest.o $(LDFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

singleImageTest.o: singleImageTest.cpp
	$(CXX) $(CXXFLAGS) -c singleImageTest.cpp

clean:
	rm -f $(TARGET) $(TARGET2) *.o
