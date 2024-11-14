# Understanding Adaptive Thresholding for Hand Gesture Detection
### Block Size Considerations

#### Small Block Sizes (e.g., 3x3 or 5x5)
* Sensitive to small variations in the image, useful for detecting fine 
details but may amplify noise.
* May blur finer details and miss smaller features.

#### Larger Block Sizes (e.g., 11x11 or 15x15)
* Softer thresholding, good for handling noisy backgrounds but might blur 
finer details.
* Suitable for larger objects.

### Testing with Different Block Sizes

* Start by testing different block sizes to see how the results change.
* Use an odd number for the block size (e.g., 3, 5, 7, 9) to ensure a 
central pixel for calculation.
* Begin with a small range (e.g., 3, 5, 7, 9) and increase it based on the 
results.

### Image Size and Resolution

* For high-resolution images, use larger block sizes (e.g., 15 or 17) to 
capture large variations in brightness.
* For smaller or lower-resolution images, use smaller block sizes (e.g., 3 or 5).

### Lighting Conditions (Surgical Setting)

* Consistent light source reduces noise impact, allowing for larger block sizes.
* Controlled environment with high contrast can help detect edges and contours more smoothly.

### Adjusting for Noise and Details

* Use block sizes around 7 to 9 for smaller hands and 11 to 15 for larger hand gestures.
* Smaller block sizes may lead to more accurate edge detection but are sensitive to noise, while larger block sizes can smooth out noise.

### Using the C Constant in Adaptive Thresholding

* The second parameter to adaptiveThreshold() is the constant C, which subtracts from the calculated mean of the pixel neighborhood.
* A smaller C allows more contrast, while a larger C reduces noise and smooths out the image.
* Probably want to adjust this along with the block size since we're trying to detect smaller details like hand gestures

### Iterating with Real-Time Testing

* Test with real-time video to determine the most effective block size.
* Start with a medium block size (e.g., 9 or 11) and adjust from there, watching how well it detects hand gestures.
* Monitor output of contours, edge detection, and gesture classification as you adjust the block size.

### Example Iteration Process

1. Start with a small block size (e.g., 5x5).
2. Observe contours or edges detected from the thresholded image.
3. Increase the block size gradually (7, 9, 11) while observing how well it handles hand gestures.
4. Adjust the constant C to fine-tune the results, particularly when background lighting conditions change.

