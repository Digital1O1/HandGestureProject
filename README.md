# Where you left off last time Feb 5
- Got better understanding of Lucas-Kanade example [DONE]
  - Keep in mind points will be tracked in the grayscale frame 
  - So adjust sliders accordingly so you can detect the hand gestures
- Next big thing to focus on : feature extraction
  - Starting with hand skeletonization
  - And learning convex hulls

## Things you're currently working on 
- Convex hulls 
- Foundation with hand skeletonization
- Feature extraction


## I. Background Subtraction and Deep Learning-based Segmentation

Improve segmentation robustness using:

*   Background subtraction
*   MediaPipe Hands or OpenPose for deep learning-based segmentation

Implement adaptive thresholding or skin color detection to make the YCrCb mask more resilient to lighting changes.

## II. Feature Extraction for Gesture Recognition

Explore contour analysis to extract hand shape and key points, including:

*   Convex hull
*   Hand skeletonization 
*   Defects to detect finger positions and count them

Use OpenCV or deep learning models for hand landmarks extraction.

## III. Machine Learning for Gesture Classification

Train a classifier (SVM, k-NN, or deep learning) to recognize specific hand gestures using:

*   CNN-based models like MobileNet or TensorFlow/Keras models for real-time gesture recognition

## IV. Robustness to OR Environment

Optimize detection under varying lighting conditions and ensure tracking remains stable despite occlusions or rapid movements by:

*   Refining feature selection
*   Filtering noisy detections
*   Reducing false positives

Reduce latency by optimizing the image processing pipeline.

### Robustness Techniques

*   Optimize detection under varying lighting conditions.
*   Ensure tracking remains stable despite occlusions or rapid movements.
*   Reduce false positives by refining feature selection and filtering noisy detections.

### Latency Optimization for Real-Time Use

Optimize the image processing pipeline for low-latency recognition using:

*   GPU acceleration (e.g., OpenCV with CUDA, TensorRT for deep learning models)
*   Limiting the region of interest to the detected han


# Things that you're curious about
- Attempting to adjust YCrCb values on the fly by 
  - Using adaptive color models instead of fixed YCrCb thresholds that use a model that samples the color of the h and at the start and adjusts dynamically 
  - Could apply `K-means clusering` or `Gaussian Mixture Model (GMM)` to segment the skin more reliably 
  - Use ML for skin detection 
    - Small CNN or DL model could recognize hands w/o relying on manual thresholding 
  - Incorporate histogram-based skin detection – Analyze the histogram of skin pixels in the first few frames and adjust thresholds dynamically.


## Keep reviewing Lucas-Kanade 
---

# Purpose of adaptive thresholding 
## What does it do
Adaptive thresholding calculates thresholds based on `local pixel intensity`, which is ideal for images with varying lighting. 

A Python code example was provided and later translated to C++ to process a video frame by frame.

## Does adaptiveThresholding help with edge detection with Canny edge detector?

Yes, adaptive thresholding helps isolate key areas, allowing Canny to detect edges more effectively. In cases with sufficient lighting, `adaptive thresholding can replace Gaussian blurring 
as a preprocessing step`, especially if noise levels are low.

---

# Motion Gesture Recognition for Surgical Training

## Step 1: Optical Flow for Motion Tracking
### Introduction to Optical Flow

Optical flow helps in tracking hand movements by analyzing pixel changes 
across frames. This technique can give valuable information about 
direction, speed, and general motion patterns of the hand.

### Methods like Lucas-Kanade and Farneback in OpenCV

Learn methods like Lucas-Kanade and Farneback optical flow in OpenCV to 
track hand motions in real-time.

## Step 2: Feature Descriptors (HOG, SIFT, SURF)
### Introduction to Feature Descriptors

Feature descriptors like Histogram of Oriented Gradients (HOG), SIFT 
(Scale-Invariant Feature Transform), or SURF (Speeded-Up Robust Features) 
provide a way to capture essential patterns in an image for detecting and 
classifying gestures.

### Choosing the Right Feature Descriptor

*   HOG is useful for detecting shapes and contours.
*   SIFT and SURF are excellent for identifying and matching specific hand 
features across frames.

## Step 3: Gesture Recognition using Machine Learning Models
### Introduction to Machine Learning Models

Now that you have a foundation in image processing, the next step is 
training a machine learning or deep learning model that can recognize 
gestures based on extracted features.

### Choosing the Right Model

*   Start with Support Vector Machines (SVM), K-Nearest Neighbors (KNN)
*   Dive into Convolutional Neural Networks (CNNs) for more complex 
classification tasks.

## Step 4: Temporal Analysis with Recurrent Neural Networks (RNNs) or 3D 
CNNs
### Introduction to Temporal Models

For complex hand gestures that unfold over several frames, temporal models 
like Long Short-Term Memory (LSTM) networks or 3D CNNs can capture 
time-based patterns.

### Benefits of Temporal Models

*   Allow for more accurate gesture classification in sequential data.

## Step 5: Data Augmentation and Preprocessing for Robustness
### Introduction to Data Augmentation

Learning about data augmentation techniques like rotations, flips, 
brightness adjustments can help improve model robustness.

### Focus on Preprocessing Techniques

Focus on preprocessing techniques that can help the model generalize 
better, especially under various lighting and positioning that may occur 
during surgery.

## Step 6: Contour and Shape Analysis
### Introduction to Shape Analysis

Investigate shape analysis techniques like convex hull, convexity defects 
that can give more structured data on hand shape.

### Combining with Contour Detection

Combining these with your contour detection can help refine gesture 
classification.



---

# Concepts for Detecting Hand Gestures with Gloves

To effectively detect hand gestures for someone wearing a glove, focus on the following concepts and techniques:

## 1. Computer Vision Basics
- **Image Processing**: Understand fundamental image processing techniques such as filtering, morphological operations, and thresholding;including adaptive thresholding.
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

## 9. Environmental Constraints in the Operating Room
- **Glove Material Properties**: Test how different glove materials reflect light under the harsh lighting conditions of an operating room. Fine-tuning methods to handle these reflections will help maintain consistent gesture detection.
- **Occlusions and Object Handling**: Surgeons often hold tools, which may cause partial occlusion of the hand. Study techniques to detect gestures even when only part of the hand is visible.
- **Complex Backgrounds**: The dynamic background in operating rooms (e.g., tools, other people) will require robust background subtraction techniques, such as adaptive background models (e.g., MOG2) and foreground masking.

## 10. Multi-Hand Detection
- **Multi-Hand Detection**: Detecting multiple hands simultaneously is important in an operating room setting. Techniques like multi-object tracking and separating which hand belongs to which person can reduce confusion.

## 11. Temporal Sequence Modeling
- **Recurrent Neural Networks (RNNs) or LSTMs**: These models may be useful for recognizing gestures that occur as part of a sequence (e.g., a surgeon giving signals in steps).

## 12. Noise and Movement Stability
- **Kalman Filtering or Particle Filtering**: In addition to Gaussian smoothing, use these filters to improve stability by accounting for real-world motion noise (jitter, accidental movements).

## 13. Gesture Calibration
- **Calibration for Different Surgeons**: Implement a calibration step to adjust for different surgeons' hand sizes and gesture preferences. This ensures the system is adaptive and responsive to the needs of different users.

## 14. Haptic Feedback and System Response
- **Haptic Feedback**: Explore haptic feedback to help surgeons confirm the recognition of gestures. This could provide immediate feedback when gestures are detected, particularly in time-sensitive situations.

## 15. Surgical Tool Tracking
- **Tool and Hand Differentiation**: Develop methods to distinguish between hand gestures and tool movements, as both may need to be tracked in certain contexts.

## 16. Gesture Safety and Robustness
- **Reducing False Positives**: In a surgical environment, incorrect gesture recognition can be critical. Consider implementing redundant gesture verification to prevent false positives or incorrect interpretations.

---
<!-- 
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
      - With video processing gaussian blur can help `in stabilizing the detected features` over time by `reducing frame to frame variations`
    - Benefit for gesture detection
      - Stable detection of hand gestures across multiple frames needed for real time applications
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
- **Gesture to Action**: -->
