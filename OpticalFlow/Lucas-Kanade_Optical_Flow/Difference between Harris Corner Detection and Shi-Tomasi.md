
# Difference between Harris Corner Detection and Shi-Tomasi


# Harris Corner Detection
## Overview

The Harris corner detector identifies corners based on the `intensity gradients within a small window` of an image. 

It determines how much the pixel intensity varies in different directions.

## Key Steps

1. Compute intensity gradients (I~x~, I~y~) of the image.
2. Construct a structure tensor (second-moment matrix) for each pixel (Indicated by M):
   a) A second movment matrix (Structure tensor)
    -  Used to analyze the local intensity structure of a image
    -  Provides info about intensity gradients in a local neighborhood of a point 
    -  Widely used for
       -  Feature detection
          -  Like in corners, edges and textures 
   ```math
    M = [
    Ix^2 
    Ix*Iy 
    Ix*Iy 
    Iy^2
    ]   
    ```



3. Compute the corner response R:
```math
R = det(M) - k * (trace(M))^2
```
   where `k` is a sensitivity parameter.

1. Determine whether to consider a point as a corner, edge, or flat region based on the value of R:
   - `Positive R`: Corners
   - `Near-zero R`: Edges
   - `Negative R`: Flat regions

2. Apply thresholding to identify corners.

## Characteristics

- Robust to noise and works well for textured or high-contrast regions.
- Sensitive to parameter tuning (e.g., `k` and threshold values).
- May detect more corners than needed, which can increase computation.

## Pros and Cons

Pros:

* Strong mathematical foundation for detecting true corners.
* Good for applications where fine-grained feature detection is essential.

Cons:

* Sensitive to scaling and rotation unless additional techniques (e.g., pyramids) are used.
* Can detect redundant features in textured areas, such as fabric or background clutter.

---


# Shi-Tomasi Corner Detection
## Overview

Shi-Tomasi builds upon Harris by refining the criterion for corner detection. Instead of using the 
determinant and trace of the structure tensor, it directly examines the eigenvalues of M.

## Key Steps

1. Compute the two eigenvalues (λ₁, λ₂) of the structure tensor M.
2. Define the response as:
   ```math
    R = min(λ₁, λ₂)
    ```

3. Apply thresholding to identify corners.

## Characteristics

- Identifies corners as regions where both eigenvalues are significant (i.e., corner-like regions).
- Provides fewer, more distinct points compared to Harris.

## Pros and Cons

Pros:

* Simple and computationally efficient.
* Produces fewer false positives in regions with high gradients but no true corners.
* More robust to noise and better suited for dynamic environments like an OR.

Cons:

* Similar to Harris, it may require additional processing for scale/rotation invariance.

---

# Comparison for OR Hand Gesture Detection

| Aspect | Harris | Shi-Tomasi |
| --- | --- | --- |
| `Detection Quality` | Robust in high-contrast areas, may over-detect | Selective, fewer and cleaner  detections |
| `Noise Robustness` | More sensitive to noise | Better noise handling |
| `Computation` | Slightly more computationally intensive | Faster and simpler |
| `Suitability for Hands` | Good for textured gloves or cluttered scenes | Ideal for dynamic, clean regions like hands |

## Recommendation

For detecting a surgeon’s hand gestures:

* `Shi-Tomasi` is likely a better choice due to its efficiency, robustness to noise, and tendency to 
return fewer, higher-quality features.

However, if the gloves or environment produce high contrast patterns, Harris may still be effective 
with careful parameter tuning.

To enhance robustness in the OR setting:

* `Apply YCrCb masking` or other preprocessing to focus on the hand.
* Consider combining these detectors with optical flow for tracking or machine learning models for 
gesture classification.
