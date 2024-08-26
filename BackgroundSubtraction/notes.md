# [Background Subtracion](https://docs.opencv.org/4.x/de/df4/tutorial_js_bg_subtraction.html)
---
# References to read
- https://www.geeksforgeeks.org/background-subtraction-opencv/
- Related to the noise that's picked up by GMG : https://docs.opencv.org/4.x/d9/d61/tutorial_py_morphological_ops.html

# Basics
## Background subtraction
- Major preprocessing steps in many CV based applications 
- Examples:
  - `Visitor counter` where a static camera takes the number of visitors entering/leaving the room
  - `Traffic camera` getting info about cars 
  - In either case
    - Need to extract person/vehicles alone
  - Technically you need to extract the `moving foreground` from the `static background`

## BackgroundSubtractorMOG2
- Gaussian mixture-based back/foreground segmentation aglorithm
- One important feature about algorithm
  - It selects the `appropriate number of gaussian distribution` for every pixel
- Provides better adaptibility to varying scenes due to illumination changes, ect.

## Can use the constructor
- ```cv.BackgroundSubtractorMOG2 (history = 500, varThreshold = 16, detectShadows = true)```

### Background Subtractor Parameters

| Parameter        | Description |
|------------------|-------------|
| **history**      | Length of the history. |
| **varThreshold** | Threshold on the squared distance between the pixel and the sample to decide whether a pixel is close to that sample. This parameter does not affect the background update. |
| **detectShadows**| If true, the algorithm will detect shadows and mark them. It decreases the speed a bit, so if you do not need this feature, set the parameter to false. |

### Returns

| Return Type | Description |
|-------------|-------------|
| **cv.BackgroundSubtractorMOG2** | Instance of the background subtractor. |

### Method: `apply`

| Parameter       | Description |
|-----------------|-------------|
| **image**       | Next video frame. Floating point frame will be used without scaling and should be in range [0,255]. |
| **fgmask**      | The output foreground mask as an 8-bit binary image. |
| **learningRate**| The value between 0 and 1 that indicates how fast the background model is learnt. Negative parameter value makes the algorithm use some automatically chosen learning rate. 0 means that the background model is not updated at all, 1 means that the background model is completely reinitialized from the last frame. |

## Note
- The instance of cv.BackgroundSubtractorMOG2 should be deleted manually.

---

## Other Background Subtraction Methods

| Method                             | Advantages                                                                                         | Disadvantages                                                                                  |
|------------------------------------|----------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
| **MOG2**                           | - Adaptive learning for changing conditions<br>- Built-in shadow detection<br>- Good for various environments | - Slightly slower processing<br>- May have false positives with slow-moving objects         |
| **KNN**                            | - Faster processing<br>- Stable background performance<br>- Better noise reduction               | - Less adaptive to dynamic backgrounds<br>- Poorer shadow handling compared to MOG2         |
| **Simple Background Subtraction**  | - Extremely fast<br>- Low complexity                                                               | - Works only with static backgrounds<br>- No adaptation to background changes                 |
| **GMG**                            | - Statistical approach combining temporal and spatial information<br>- Adaptive                   | - Requires initial learning phase<br>- More computationally intensive                        |
| **CNT**                            | - Very efficient and optimized for real-time scenarios<br>- Good balance of speed and accuracy    | - Less robust to dynamic changes in the environment                                           |
| **Legacy MOG**                     | - Proven method<br>- Customizable number of Gaussian mixtures                                    | - Outdated compared to MOG2<br>- Less adaptability to dynamic environments                    |

## Game plan with testing B.S methods
- Write program that applies each one 
  - cv::createBackgroundSubtractorMOG2()
  - cv::bgsegm::createBackgroundSubtractorGMG()
  - cv::bgsegm::createBackgroundSubtractorCNT()
  - cv::bgsegm::createBackgroundSubtractorLSBP()
  - cv::createBackgroundSubtractorKNN()
- Display FPS and CPU usage
  - Also have whatever computer meterics written to CSV file to showcase data
- Use some random video to be used as a `performance benchmark` for consistency sake
  - Using Matrix clip 

## More game planning with B.S testing methodlogy
### 1. Capture Your Own Dataset
- [ ] **Why It's Important**: Understand the importance of using real-world data to test and refine your model.
- [ ] **How to Do It**:
  - [ ] Use a webcam or the camera on your Raspberry Pi 5 to capture video clips of hand gestures.
  - [ ] Record a variety of gestures with different lighting conditions, hand sizes, and backgrounds.
  - [ ] Store these videos as your initial dataset.

### 2. Preprocessing
- [ ] **Objective**: Clean and prepare your data for modeling.
  - [ ] **Background Subtraction**: Apply the background subtraction methods you've tested.
  - [ ] **Normalization**: Adjust brightness and contrast to make the gestures more consistent.
  - [ ] **Resizing**: Resize frames to a standard size to reduce computation and ensure consistency.

### 3. Feature Extraction
- [ ] **What It Is**: Learn to extract features that describe the gesture.
  - [ ] **Contours**: Use OpenCV’s `findContours` to detect the boundary of the hand.
  - [ ] **Convex Hull**: Use OpenCV’s `convexHull` to detect the outermost points of the hand.
  - [ ] **Key Points**: Explore key point detection methods like SIFT, SURF, or ORB.

## 4. Modeling
- [ ] **Objective**: Build a model that can recognize gestures from the extracted features.
  - [ ] Start with simple models like k-Nearest Neighbors (k-NN) or Support Vector Machines (SVM).
  - [ ] Explore more complex models like Convolutional Neural Networks (CNNs) as you gain more experience.

### 5. Evaluation and Iteration
- [ ] **Evaluate**: Test your model with different gestures and conditions. Assess accuracy, speed, and robustness.
- [ ] **Iterate**: Go back and tweak your preprocessing, feature extraction, or model parameters based on your evaluations.

### 6. Optimization for Raspberry Pi 5
- [ ] **Why It’s Critical**: Ensure your model is optimized for the Raspberry Pi 5's limited resources.
- [ ] **How to Do It**:
  - [ ] Consider using lighter models or pruning techniques.
  - [ ] Optimize your code to make the most out of the hardware, potentially using tools like OpenCV's GPU acceleration (if applicable).