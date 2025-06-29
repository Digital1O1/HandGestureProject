# Convex Hulls
- Smallest convex polygon that encloses all the ppoints in that shape 
- Think of a rubber band around your hand
    - The path the band takes is the convex hull

# With OpenCV
- `hull`
    - The outer points of the shape 
    - Could skip between fingers 
- hullIndices 
    - The indices in the original contour that forms the hull 
    - Used for convexity defects
```cpp
cv::convexHull(contour, hull); // Returns convex hull points
cv::convexHull(contour, hullIndices); // Returns indices into the original contour
```

# Convexity Defects 
- Deepest point between two fingers 
    - How far the actual contour dips inward relative to the convex hull
- OpenCV defines a convexity defect as a `start point`, `end point`, `far point`, and `depth` 
- `Depth` 
    - The distance between the `farthest inward point` (usually between two fingers) and the `convex hull line`

# In OpenCV
```cpp
cv::convexityDefects(contour, hullIndices, defects);

// Each defect is a Vec4i
start_index = defect[0] // Where the convex hull segment starts (base of one finger)
end_index   = defect[1] // Where it ends (The base of the finger)
far_index   = defect[2] // The deepest point between the two (webbing between fingers)
depth       = defect[3] / 256.0  // fixed-point format, so divide by 256

```

# How Are Convex Hulls & Defects Intertwined?
## Here’s the step-by-step relationship:

- `Contour`: You detect the hand using `thresholding` and `cv::findContours()`.
- `Convex Hull`: You find the `outermost point`s that wrap around the hand.
- `Defects`: You look at the `"dents"` between those convex hull points.
- `Depth`:
  -  If the hand is open, deep "valleys" form between the fingers.
    - If the hand is closed (fist), the defects will be fewer and shallower.
  -You can filter defects by `depth > threshold` to ignore noise or barely-visible indentations.