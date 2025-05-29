CXX = g++ 
CXXFLAGS = -std=c++11 `pkg-config --cflags opencv4 yaml-cpp`
LDFLAGS = `pkg-config --libs opencv4 yaml-cpp`

TARGET = preconfigureCameraSettings 
SRC = main.cpp

$(TARGET) : $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)
	
clean :
	rm -f $(TARGET)	