# Reference link 
https://docs.opencv.org/4.10.0/d4/d73/tutorial_py_contours_begin.html

# What are contours
Curves joining all the `continuous points along the boundary` that have the same `color` or `intensity` 
Contours are a useful tool for 
- Shape analysis
- Object detection 
- Recognition

For better accuracy
- Use binary images
- Before finding contours 
  - First apply a `threshold` 
  - Or `Canny edge detection`

## Contour approximation method
The third argument in `cv.findContours` 
- We're told earlier the contours are `boundaries of a shape with the same intensity` 
- The contours store the `(x,y)` coordinates of the boundary of a shape
  - The big question is `does it store ALL the coordinates`
  - ^ That is specified by the `contour approxximation method` 
    - `cv.CHAIN_APPROX_NONE` : saves all the boundary points 
      - But the big question you have to ask is that do we really need all those points?
      - Take a line for example, we probably just need the `two end points` 
      - That is where `cv.CHAIN_APPROX_SIMPLE` comes into play and saves memory

Below are two rectangels that demonstrate this technique 
- A circle is drawn on all the coordinates in the contour array that's drawn in blue
- First image : cv.CHAIN_APPROX_NONE 
  - 734 points
- Second image : cv>CHAIN_APPROX_SIMPLE
  - Only 4 points 

    ![alt text](Screenshots/rectangles.jpg)