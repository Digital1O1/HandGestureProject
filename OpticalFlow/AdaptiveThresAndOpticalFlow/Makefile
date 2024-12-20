CXX = g++
CXXFLAGS = -std=c++11 -g `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`
TARGET = adaptiveThresholdAndOpticalFlow
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)


# WRONG ATTEMPT
# CXX = g++
# CXXFLAGS = -std=c++11 -g 'pkg-config --cflag opencv4'
# LDFLAGS= 'PKG_CONFIG --libs opencv4'
# TARGET = adaptiveThresholdAndOpticalFlow 
# SRC = main.cpp
# all = $(TARGET)

# $(TARGET): $(SRC)
# 	$(CXX) $(CXXFLAGS) -o $(SRC) $(LDFLAGS)

# clean:
# 	rm -f $(TARGET)