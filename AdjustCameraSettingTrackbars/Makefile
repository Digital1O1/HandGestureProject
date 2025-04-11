CXX = g++ 			# State which compiler to use
CXXFLAGS = -std=c++11 `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

TARGET = adjustCamera 
SRC = main.cpp

$(TARGET) : $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)
	
clean :
	rm -f ($TARGET)	# State which C++ flags are needed