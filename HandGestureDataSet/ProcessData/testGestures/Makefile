CXX = g++
CXXFLAGS = -std=c++17 -O2

# ONNX Runtime path (adjust if needed)
# find ~ -type d -name "onnxruntime-linux-x64-gpu-1.17.1" 2>/dev/null

ONNX_DIR = /home/digital101/onnxruntime-linux-x64-gpu-1.17.1

# CUDA paths (adjust if non-standard)
CUDA_HOME = /usr/local/cuda-12.3

# Include and lib flags
INCLUDES = -I$(ONNX_DIR)/include -I$(CUDA_HOME)/include
LIBS = -L$(ONNX_DIR)/lib -lonnxruntime -lonnxruntime_providers_cuda
LIBS += -L$(CUDA_HOME)/lib64 -lcudart
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lyaml-cpp -lstdc++fs
# RPATH to avoid needing LD_LIBRARY_PATH manually
RPATH = -Wl,-rpath=$(ONNX_DIR)/lib


# OpenCV flags
OPENCV_CFLAGS = `pkg-config --cflags opencv4`
OPENCV_LIBS = `pkg-config --libs opencv4`

TARGET = gesture_detector
SRC = main.cpp


all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(OPENCV_CFLAGS) $(INCLUDES) $(SRC) -o $(TARGET) $(OPENCV_LIBS) $(LIBS) $(RPATH)

clean:
	rm -f $(TARGET)