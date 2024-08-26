# References 
- https://nalinc.github.io/blog/2018/skin-detection-python-opencv/
- https://github.com/CHEREF-Mehdi/SkinDetection


# Grayscale Detection Approaches:
- [ ] **Background Subtraction:** Use background subtraction techniques to isolate moving objects (like hands) in a grayscale video. This method focuses on detecting changes in the scene rather than identifying specific colors.
  
- [ ] **Edge Detection:** Apply edge detection methods (e.g., Canny edge detection) to identify the contours of a hand in a grayscale image. This can be effective for gesture recognition, especially if the background is relatively uniform.
  
- [ ] **Optical Flow:** Use optical flow methods to track the movement of pixels between frames in a video sequence. This can be used to detect hand movements in grayscale images without relying on color information.
  
- [ ] **Shape and Contour Analysis:** After detecting motion, analyze the shape and contours of the moving object to differentiate between a hand and other objects.

### Performance Considerations on Raspberry Pi:
- [ ] **Grayscale Processing:** Grayscale images require less computational power than color images, which is beneficial on a Raspberry Pi. The reduced data size also speeds up processing.
  
- [ ] **Trade-offs:** While you lose the ability to detect skin tones directly, the faster processing might allow for real-time gesture recognition, especially if combined with other features like motion tracking.
