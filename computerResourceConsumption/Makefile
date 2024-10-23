CXX = g++ 
CXXFLAGS = -std=c++11 
# LDFLAGS = `pkg-config --libs opencv4`

TARGET = computerResources
SRCS = main.cpp

all: $(TARGET)

# $(TARGET): $(SRCS)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) 

clean:
	rm -f $(TARGET)

# make -j$(nproc)
