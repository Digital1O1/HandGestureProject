CXX = g++
# CXXFLAGS = -std=c++11 -I/usr/include/opencv4 -I/usr/local/include
# LDFLAGS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lyaml-cpp

CXXFLAGS = -std=c++17 `pkg-config --cflags opencv4 yaml-cpp` 
LDFLAGS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lyaml-cpp -lstdc++fs
TARGET = checkDuplicate
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(TARGET)
