# Plan of Action for Detecting Surgeon Hand Gestures in an Operating Room (OR)

Detecting hand gestures in an operating room (OR) involves overcoming several challenges related to lighting, occlusion, noise, and the precision required in such a sensitive environment. 

Below is a structured plan of action, including things you should read up on and understand to effectively detect a surgeon’s hand gestures.

## 1. Understand the Problem and Set Requirements
### Key Points:
- **Surgeon’s Hand Gestures**: Focus on detecting precise and intentional hand gestures, not just general hand movement.
- **Environmental Factors**:
  - Low and dynamic lighting.
  - Occasional occlusion (hands obscured by instruments or clothing).
  - Need for real-time processing.
  - Possible presence of gloves.
- **Gestures to Detect**: Clarify the types of hand gestures or movements to detect (e.g., specific finger movements, hand orientation, or specific gestures like pointing, holding, or swiping).

### Tasks:
- Define specific hand gestures that need to be tracked (e.g., swiping, pointing, or grasping gestures).
- Determine the level of gesture recognition needed (simple tracking vs. complex gesture classification).

## 2. Computer Vision and Optical Flow
Understanding and applying optical flow and feature tracking will be crucial in tracking hand movements.

### Tasks:
- **Learn about Optical Flow**:
  - Study the Lucas-Kanade method for optical flow (used for tracking points between frames).
  - Understand how to handle large motions, noise, and occlusion in optical flow-based methods.
  - Read about dense optical flow vs. sparse optical flow to choose the best option for your use case.

### Resources:
- OpenCV documentation on Optical Flow.
- Research papers on optical flow in complex environments.

- **Feature Detection**:
  - Study `goodFeaturesToTrack` and other feature detectors (Harris, Shi-Tomasi) to find robust points to track.
  - Understand how to choose the right number of points and deal with feature loss during occlusion.

### Resources:
- OpenCV `goodFeaturesToTrack` documentation.
- "Feature Detection: Computer Vision: Algorithms and Applications."

- **Track the Hand Using Optical Flow**:
  - Focus on tracking points within a region of interest (ROI) (e.g., the hand or gloves area).
  - Apply smoothing (Kalman Filter, temporal filtering) to ensure stable tracking.

## 3. Handling Occlusion and Motion Artifacts
Gestures may be occluded by surgical tools or parts of the surgeon’s body.

### Tasks:
- **Study Occlusion Handling**:
  - Learn about techniques to track under occlusion (such as tracking only parts of the hand or using deep learning to fill in gaps).
  - Understand how to filter out occluded points using status flags from optical flow.

### Resources:
- Kalman Filter for Tracking in OpenCV for better handling of tracking errors during occlusion.
- Papers on tracking occlusion in video.

- **Study Gesture Detection Techniques**:
  - Learn how to detect gestures even with partial visibility (e.g., tracking specific joints or hand landmarks).
  - Study the use of hand pose estimation models to track the entire hand even under partial occlusion.

### Resources:
- Hand Tracking with MediaPipe: Google’s MediaPipe library has an excellent hand tracking solution that could be used for robust hand gesture recognition.
- Real-time Hand Tracking with MediaPipe.

## 4. Real-Time Gesture Recognition
Once you have stable tracking, you need to process and classify gestures in real-time.

### Tasks:
- **Real-Time Gesture Recognition**:
  - Research gesture recognition models that can recognize specific hand movements from tracked points.
  - Consider using Hidden Markov Models (HMMs) or Long Short-Term Memory (LSTM) networks for temporal gesture recognition.

### Resources:
- Gesture Recognition using Machine Learning: Learn about supervised and unsupervised learning for gesture recognition.
- LSTM for time series data in gesture recognition.

### Resources:
- Gesture Recognition with Deep Learning.
- Real-time Hand Gesture Recognition with LSTM.

- **Train a Gesture Recognition Model**:
  - Collect a dataset of hand gestures performed in an OR-like environment, or use publicly available datasets.
  - Preprocess the data, normalize it, and feed it to a gesture recognition system for training.

- **Real-Time Inference**:
  - Ensure your system can process gestures in real-time with low latency.
  - Use optimized algorithms for efficient inference (e.g., quantized models for faster prediction).

### Resources:
- OpenCV real-time processing with multi-threading.
- TensorFlow Lite or ONNX for efficient model deployment.

## 5. Gesture Mapping to Actions in OR
### Tasks:
- **Action Mapping**: Map detected gestures to actions that could control devices in the OR.
- **Implement Action Triggers**: Depending on the gesture, implement action triggers (e.g., control surgical tools, adjust lighting).
- **Consider Safety**: Ensure that gesture recognition doesn't lead to misinterpretation in critical environments. Implement validation mechanisms where necessary.

## 6. Integration and Testing
### Tasks:
- **Integrate System into OR Setup**: Implement the system with a live camera feed and test in a controlled OR-like environment.
- **Test in Various Lighting Conditions**: Test robustness in different lighting conditions (e.g., shadows, low light).
- **Evaluate Gesture Recognition Accuracy**: Evaluate precision and recall in gesture detection.
- **Collect Feedback from Surgeons**: Collect feedback on the system's usability and accuracy in real-world operating room scenarios.

### Resources:
- OpenCV for Camera Integration and Real-Time Video Processing:
  - Study OpenCV’s camera integration and real-time video processing capabilities.
  - OpenCV Video I/O.

## 7. Final Considerations and Challenges
### Tasks:
- **Refine Algorithm**:
  - Implement error recovery methods if the tracking or gesture detection fails.
  - Use machine learning-based recovery to detect when the tracker loses the hand and reinitialize tracking automatically.
- **Safety and Privacy**: Ensure all data is handled in a secure and compliant manner, especially if working with real OR setups.

## Summary of Resources to Read and Understand:
- **OpenCV Documentation**:
  - Optical Flow, `goodFeaturesToTrack`, and feature tracking.
  - [OpenCV Optical Flow](https://docs.opencv.org/4.x/d4/dee/tutorial_optical_flow.html)
  - Kalman Filter.

- **Machine Learning for Gesture Recognition**:
  - Gesture Recognition with LSTMs or HMMs.
  - Gesture Recognition with Deep Learning.
  - LSTM for Gesture Recognition.

- **Hand Tracking with MediaPipe**:
  - Real-time Hand Tracking with MediaPipe.

- **Advanced Computer Vision**:
  - Feature detection and handling occlusion.
  - Advanced optical flow techniques.
  - Tracking algorithms and smoothing filters.

By following this plan and focusing on the appropriate areas, you will be well-equipped to develop an accurate and robust hand gesture detection system for a surgical setting.


# Optical Flow Notes

## Eculidean distance 
### What is it
- `Straight line distance` between two points on a multidimensional space 
- In 2D
  - It's the lenght of the **shortest path** between two points on a plane
- Is computed using the  **Pythagorean theorem**

### Eculidean distance calculations
#### 2D Space
For two points \( P_1(x_1, y_1) \) and \( P_2(x_2, y_2) \), the Euclidean distance \( d \) is given by:

\[
d = \sqrt{(x_2 - x_1)^2 + (y_2 - y_1)^2}
\]

---

#### 3D Space
For points \( P_1(x_1, y_1, z_1) \) and \( P_2(x_2, y_2, z_2) \), the formula extends to:

\[
d = \sqrt{(x_2 - x_1)^2 + (y_2 - y_1)^2 + (z_2 - z_1)^2}
\]

---

#### Higher Dimensions
For an \( n \)-dimensional space, where the coordinates of two points are \( (x_1, x_2, \dots, x_n) \) and \( (y_1, y_2, \dots, y_n) \), the generalized formula is:

\[
d = \sqrt{\sum_{i=1}^{n} (x_i - y_i)^2}
\]## cv::goodFeaturesToTrack(prevGray, prevPoints, 100, 0.3, 7);

### Function Syntax 
```c++
void cv::goodFeaturesToTrack	(	
InputArray image,
OutputArray corners,
int 	maxCorners,
double 	qualityLevel,
double 	minDistance,
InputArray 	mask = noArray(),
int 	blockSize = 3,
bool 	useHarrisDetector = false,
double 	k = 0.04 
)	
```

**How It Works**
===============

### Input Parameters

| Parameter | Description |
| --- | --- |
| Image | The source grayscale image. |
| Max Corners | The maximum number of corners to detect. |
| Quality Level | A value (0 to 1) representing the minimum corner quality 
relative to the strongest detected corner. |
| Min Distance | The minimum distance allowed between detected corners to 
avoid redundancy. |
| Mask (optional) | Defines a region of interest for corner detection. |
| Block Size | The size of the neighborhood used to calculate gradients and the structure tensor. |
| Harris Detector (optional) | Use Harris detector instead of Shi-Tomasi if set to true. |
| K (optional) | Harris detector parameter (used only if Harris is enabled). |

### Steps in the Function
1. **Gradient Calculation:**
   - Computes the horizontal (\(I_x\)) and vertical (\(I_y\)) gradients of the image.

2. **Structure Tensor Creation:**
   - Forms a 2x2 matrix \(M\) (structure tensor) at each pixel based on the gradients.

3. **Corner Measure Calculation:**
   - Uses the Shi-Tomasi or Harris corner detection algorithm:
     - **Shi-Tomasi:** \(\lambda_{\text{min}} = \min(\lambda_1, \lambda_2)\), where \(\lambda_1\) and \(\lambda_2\) are the eigenvalues of \(M\).
     - **Harris:** Computes \(R = \text{det}(M) - k(\text{trace}(M))^2\).

4. **Non-Maximum Suppression:**
   - Retains only the strongest corners within a neighborhood (default 3x3).

5. **Thresholding and Sorting:**
   - Discards corners below the quality threshold.
   - Sorts remaining corners in descending order of quality.

---

## Important Things to Know for Hand Gesture Detection

### Choosing Parameters
- **Quality Level:** A low value (e.g., 0.01) allows more corners to be detected but may introduce noise. For precise tracking, experiment to find the right balance.
- **Min Distance:** Prevents closely packed corners, reducing redundancy. This is useful for isolating distinct hand regions.
- **Max Corners:** Limit the number of corners to focus on the most prominent ones, especially for real-time processing.

### Preprocessing
- Convert the input frame to grayscale before calling the function.
- Apply noise reduction (e.g., Gaussian blur) to avoid detecting false corners due to noise.
- Mask irrelevant regions (e.g., background) to limit detection to the hand.

### Applications to Hand Gesture Detection
- **Finger Tip Detection:** Corners are often detected at the fingertips, which can help identify gestures.
- **Hand Contour Tracking:** Corners along the hand boundary can aid in defining its shape.
- **Motion Tracking:** Combine corner detection with **optical flow** to track movements of the hand and fingers between frames.

### Limitations
- Works best in images with well-defined edges and gradients. It may fail under poor lighting or excessive motion blur.
- May detect irrelevant features if the background contains strong gradients or textures.


---

# Difference between Harris Corner Detection and Shi Thomas

## Harris Corner Detection

### Method:
Based on the response of a matrix derived from image gradients, identifying locations where intensity varies significantly in both:
- **x** and **y** directional changes.

### Strengths:
- Detects corners effectively and robustly.
- Finds more "significant" features due to its reliance on eigenvalues and a threshold.
- Useful in relatively static environments.

### Weaknesses:
- Prone to over-detection in high-texture areas, which can include noise.
- Does not rank corners, leading to challenges when selecting the most useful points.
- Sensitive to the **k** parameter (empirical constant), which may require fine-tuning.

---

## Shi-Tomasi Corner Detection

### Method:
Improves upon Harris by focusing on the minimum eigenvalue of the gradient matrix, ensuring only strong corners are retained.

### Strengths:
- Better feature ranking by directly using the eigenvalues for quality assessment.
- More robust and selective than Harris in high-motion environments.
- Adaptive to real-world applications with dynamic lighting and motion, such as tracking a surgeon’s hand gestures.

### Weaknesses:
- Slightly higher computational cost due to eigenvalue sorting.
- Still requires parameter tuning (e.g., `qualityLevel` and `minDistance`).

---

## Comparison for Detecting Hand Gestures in an OR

Shi-Tomasi is generally better suited for dynamic and complex scenarios like detecting hand gestures in an operating room:

### Dynamic Environment:
Hand gestures involve constant motion and shape changes; Shi-Tomasi handles such situations with greater stability.

### Feature Quality:
Its eigenvalue-based ranking helps prioritize meaningful points, reducing noise in gesture tracking.

### Selective Detection:
Limits over-detection in areas with high texture, such as surgical instruments or gloves with fine patterns.

If computational efficiency or simplicity is a priority and the environment is more controlled (e.g., no dramatic lighting or motion variations), Harris could still be an option. However, Shi-Tomasi offers a better balance between robustness and adaptability for OR use cases.

<!-- 
## Parameter overview

### Image Input

*   `image`: 8-bit or floating-point 32-bit, single-channel image.

### Detection Output Parameters

#### Corner Features

*   `corners`: Output vector of detected corners.
*   `maxCorners`:
    *   Maximum number of corners to return. If there are more corners 
than are found, the strongest of them is returned.
    *   `maxCorners <= 0` implies that no limit on the maximum is set and 
all detected corners are returned.

#### Quality Level

*   `qualityLevel`: Parameter characterizing the minimal accepted quality 
of image corners.
*   The parameter value is multiplied by the best corner quality measure, 
which is the minimal eigenvalue (see 
[cornerMinEigenVal](#cornerMinEigenVal)) or the Harris function response 
(see [cornerHarris](#cornerHarris)).
*   The corners with the quality measure less than the product are 
rejected. For example, if the best corner has the quality measure = 1500, 
and the qualityLevel=0.01 , then all the corners with the quality measure 
less than 15 are rejected.

#### Euclidean Distance

*   `minDistance`: Minimum possible Euclidean distance between the 
returned corners.

### Optional Mask

*   `mask`: Optional region of interest.
*   If the image is not empty (it needs to have the type CV_8UC1 and the 
same size as image ), it specifies the region in which the corners are 
detected.

#### Block Size and Detector Parameters

*   `blockSize`: Size of an average block for computing a derivative 
covariation matrix over each pixel neighborhood.
*   See [cornerEigenValsAndVecs](#cornerEigenValsAndVecs).
*   `useHarrisDetector`:
    *   Parameter indicating whether to use a Harris detector (see [cornerHarris](#cornerHarris)).
*^_   `k`: Free parameter of the^_ Harris detector.


### Details about parameters 
- TL;DR this function determines `strong corners` on an image
- Function finds most prominent corners in the image/region 
- Function calcualtes `corner quality` measured at every source image pixel using 
  - `cornerMinEigenVal`
    - Simpiler but effective measure of corner strenght based on the eigenvalues of the gradient matrix
  - `cornerHarris`
    - Evaluates how well a windowed region of pixels response to shifts in intensity gradients 
- Function also performs `non-maximum suppression`
  - What does that mean?
  - Non-maximum suppression ensures `only the most prominent corners` in a local region are retained after filtering for a specific feature by retaining only the strongest response in a local neighborhood
    - Corners with the `minimal eigenvalue` less than `qualityLevel⋅maxx,yqualityMeasureMap(x,y)` ar rejected
    - Remaining corners are sorted by the quality measure in the descending order
  - Function throws away each corner if there's a stronger corner `at a distance less than maxDistance`
- This function can be used to initialize a `point-based tracker` of an object -->