# Areas you're currently working on:
- Contour detection using `Canny edge detection` in the `DetectingContoursInGeneral` folder
  - Added skin detection stuff with HSV values
  - Have to try with YCrBr values since it's more robus to lighting variations  

# Things to focus on
- Since the surgeon is wearing a glove, don't worry about detecting skin and focus on 
  - Analyzing shapes
  - Contours 
  - Movement
---

# Concepts for Detecting Hand Gestures with Gloves

To effectively detect hand gestures for someone wearing a glove, focus on the following concepts and techniques:

## 1. Computer Vision Basics
- **Image Processing**: Understand fundamental image processing techniques such as filtering, morphological operations, and thresholding.
- **Contour Detection**: Familiarize yourself with detecting and analyzing contours in images, which will help identify the shape of the hand.

## 2. Feature Detection
- **Keypoint Detection**: Learn about methods like ORB (Oriented FAST and Rotated BRIEF) or SIFT (Scale-Invariant Feature Transform) to identify distinctive points on the glove that can be used for gesture recognition.
- **Shape Descriptors**: Study how to describe the shape of the hand using features like Hu Moments or shape histograms.

## 3. Gesture Recognition Techniques
- **Template Matching**: Understand how to compare current frames to predefined gesture templates.
- **Machine Learning**: Learn basic machine learning concepts and classifiers (like KNN, SVM) to classify gestures based on extracted features.

## 4. Depth and Motion Tracking (Optional)
- **Optical Flow**: Study how to track motion across frames, which can be helpful for recognizing gestures based on movement patterns.
- **Depth Sensors**: If available, explore using depth cameras to capture hand positions and movements more accurately.

## 5. Color and Texture Analysis
- **Color Segmentation**: Since gloves can be of various colors, understanding how to segment colors effectively will be important. This might include using color spaces like HSV for better segmentation.
- **Texture Analysis**: Learn to analyze texture patterns that may help distinguish between different gloves and hand gestures.

## 6. Neural Networks and Deep Learning (Optional)
- **Convolutional Neural Networks (CNNs)**: If you want to explore more advanced gesture recognition, studying CNNs can be beneficial. They are particularly effective for image classification tasks.
- **Transfer Learning**: Consider using pre-trained models to speed up the process of training a gesture recognition system.

## 7. Real-Time Processing
- **Frame Rate Optimization**: Learn techniques for processing video frames efficiently to ensure real-time performance. This might include reducing image resolution, skipping frames, or using efficient algorithms.
- **Latency Reduction**: Understand how to minimize latency between gesture detection and the resulting action (like controlling PWM signals).

## 8. Microcontroller Integration
- **PWM Control Basics**: Familiarize yourself with how to control PWM signals to drive an LED based on detected gestures.
- **Microcontroller Programming**: Understand how to program microcontrollers (like Arduino or Raspberry Pi) to interact with the computer vision system.

## Summary
Focusing on these concepts will equip you with the knowledge and skills needed to detect hand gestures while someone is wearing a glove. You may not rely on skin detection but rather on shape, motion, and features that distinguish gestures effectively. Start with the basics and progressively dive into more advanced techniques as you build your project.


---
# Hand Gesture Detection for Incrementing/Decrementing Integer Variable

To write a C++ program that detects subtle hand gestures for controlling an integer variable, you need to understand several fundamental concepts in image processing. Here are the key areas you should focus on:

# Key areas of focus

## 1. Image Acquisition
- **Capture Devices**: Understand how to interface with cameras or video capture devices using libraries like OpenCV.
  - DONE
- **Video Frames**: Learn to read video frames from the capture device.
  - DONE

## 2. Image Preprocessing
- **Grayscale Conversion**: Converting color images to grayscale to simplify the processing.
  - DONE
- **Noise Reduction**: Techniques like Gaussian Blur to reduce noise in the image.
  - Gaussian Blur
    - Smoothing filter 
    - Reduces image noise/detail
    - How it works
      - Averages pixel in a neighborhood defined by `kernal size` and `standard deviation (sigma)`
      - By blurring image 
        - Can reduce high-frequency noise and small variations that could interfere with `contour detection` and `feature extraction`
  - Edge detection
    - `Sobel` | `Laplacian` | `Canny Edge Detection`
      - Can use any of these to detect the edges within an image
      - Blurring beforehand can make these edge detectors more `robust` 
        - `Smooths out noise` that could otherwise be detected as `false edges` 
    - Benefits for gesture detection
      - Clear edges needed for detecting boundaries of the hand 
      - Blurring helps create more distinct separation between 
        - The hand
        - The background
      - Thus improving the reliability of edge detection algorithms
  - Contour Detection
    - Threshold and Blurring
      - Thresholding : converts grayscale image to binary image
        - Pixels black or white
      - Applying `Gaussian blur` BEFORE thresholding
        - Helps reducing the impact of small variations in pixel intensity 
        - Leads to cleaner/more continuous contours
    - Benefit for gesture detection
      - For hand detection, clean contours is `crucial` for recognizing shape/features of the hand
      - `Blurred images` lead to better `defined contours`
        - Thus making subsequent contour analysis more accurate
  - Stability in tracking
    - Temporal smoothing
      - With video processing gaussian blur can help `in stabailizing the detected features` over time by `reducing frame to frame variations`
    - Benefit for gesture detection
      - Stable detection of hand gesutres across multiple frames needed for real time appliactions
      - Smoothing helps with achieving `consistent detection`, `reducing jitter` and `sudden changes` caused by noise
- **Thresholding**: Converting grayscale images to binary images using techniques like Otsu's method.

## 3. Hand Detection
- **Background Subtraction**: Techniques to isolate the hand from the background.
  - DONE
- **Skin Color Detection**: Using color segmentation to detect skin tones.
  - DONE
- **Contour Detection**: Finding contours that represent hand shapes using methods like `findContours` in OpenCV.
  - CURRENTLY WORKING AS OF 10/10/24

### Side quest for background subtraction
- [OpenCV reference](https://docs.opencv.org/3.4/d1/dc5/tutorial_background_subtraction.html)
- In a nutshell, background subtraction calculates the `foreground mask` by subtracts the `current frame` from the `background model`
- Background modeling consists of two steps
  - Background initialization
    - Initial model of the background is computed
  - Background update
    - Model is updated in order to adapt to possible changes in the scene

## 4. Feature Extraction
- **Key Points**: Detecting key points like fingertips and knuckles.
- **Convex Hull and Defects**: Using convex hulls to detect the shape of the hand and identifying defects to recognize gestures.

## 5. Gesture Recognition
- **Template Matching**: Matching detected shapes against pre-defined templates.
- **Machine Learning**: Using machine learning models (like SVM, CNN) to recognize more complex gestures.

## 6. Post-Processing
- **Smoothing**: Smoothing the detected gestures to avoid sudden jumps in the recognized gesture.

## 7. Action Mapping
- **Gesture to Action**: Mapping recognized gestures to specific actions like incrementing or decrementing an integer variable.

## 8. Performance Optimization
- **Real-time Processing**: Ensuring that your program can process frames in real-time.
- **Efficient Algorithms**: Using efficient algorithms and possibly leveraging hardware acceleration.

# Other concepts you have to understand 

## 1. Feature Extraction
- **Key Points and Landmarks**: Identify and track specific points on the hand, such as fingertips and knuckles. Techniques include Haar cascades, Histograms of Oriented Gradients (HOG), and MediaPipe Hand model.
- **Descriptive Features**: Extract features like hand shape, size, and orientation to distinguish between gestures.

## 2. Gesture Recognition
- **Template Matching**: Compare detected hand shapes against predefined gesture templates.
- **Machine Learning Models**: Use models like Support Vector Machines (SVMs), Neural Networks (NNs), and Convolutional Neural Networks (CNNs). Deep learning frameworks such as TensorFlow and PyTorch are useful for training complex models.
- **Dynamic Time Warping (DTW)**: Recognize temporal gestures by comparing sequences of hand positions over time.

## 3. Background Subtraction
- **Static Background Subtraction**: Techniques like Gaussian Mixture Models (GMM) to differentiate the hand from a static background.
- **Adaptive Background Subtraction**: For dynamic environments where the background changes over time.

## 4. Image Segmentation
- **Thresholding**: Use adaptive or global thresholding to segment the hand from the background.
- **Region Growing**: Segment regions based on pixel connectivity and similarity.

## 5. Tracking Algorithms
- **Object Tracking**: Track the hand’s movement using algorithms like Kalman Filters, Mean Shift, or Optical Flow.
- **Multiple Object Tracking**: For scenarios with multiple hands or gestures being tracked simultaneously.

## 6. Depth Sensing
- **3D Gesture Recognition**: Utilize depth cameras (e.g., Microsoft Kinect or Intel RealSense) to capture 3D hand movements for more accurate recognition.
- **Point Cloud Processing**: Analyze 3D point clouds for gesture recognition.

## 7. Real-Time Processing
- **Optimization Techniques**: Ensure efficient real-time performance by using optimized algorithms and parallel processing.
- **Hardware Acceleration**: Utilize GPUs or specialized hardware to speed up image processing tasks.

## 8. Post-Processing
- **Filtering**: Apply smoothing filters to reduce noise and jitter in detected gestures.
- **Gesture Validation**: Implement logic to validate and confirm gestures to minimize false positives.

## 9. User Interface Integration
- **Feedback Mechanism**: Provide visual or auditory feedback based on recognized gestures.
- **Control Mapping**: Map recognized gestures to specific actions or controls in your application.

## Example Workflow for Real-Time Hand Gesture Detection

1. **Capture Frames**: Continuously capture frames from a camera.
2. **Preprocess Frames**: Convert to grayscale, apply Gaussian blur, and threshold the image.
3. **Detect and Segment Hand**: Use background subtraction or segmentation to isolate the hand.
4. **Extract Features**: Detect key points and extract features relevant to gesture recognition.
5. **Recognize Gesture**: Compare features against predefined gestures using machine learning or template matching.
6. **Update UI/Controls**: Map recognized gestures to actions and update the user interface accordingly.

## Resources and Tools

- **OpenCV**: For image processing and basic object detection.
- **TensorFlow/PyTorch**: For training and using deep learning models.
- **MediaPipe**: For pre-built models and pipelines for hand and pose detection.
- **Depth Cameras**: For enhanced gesture recognition with depth data.
