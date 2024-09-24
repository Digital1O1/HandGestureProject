# [Morphological Transformations](https://docs.opencv.org/4.x/d9/d61/tutorial_py_morphological_ops.html)

## Theory 
- What do `morphological transformations` do?
  - Simple operations based on `image shape`
- Normally performed on `binary images`
- Just needs two inputs
  - The original imge
  - The `structuring element` or `kernel` 
    - Decides the nature of operation
- Two basic morphological operators 
  - Erosion
  - Dilation
  - Other variants
    - Opening 
    - Closing
    - Gradient

## Erosion  
- Just like with soil erosin
  - It erodes away the boundaries of a `foreground object`
    - Always tries to keep foreground in `white` 
- So what does it do?   
  - The `kernel` slides through an image as a `2D convolution` 
  - What is a convolution?
    - Fundamental operation used to apply filters to an image
    - Takes a small matrix called a kernel and slides it across the entire image 
      - To perform element-wise multiplication and summing the results to produce a new value at each position
  - A pixel in the `original image` that's represented as a `1` or a `0`
    - Will only be considered a `1` if all the pixels under the kernel is 1
    - Otherisde it's eroded and made to `0`
- So...
  - All the pixels near the boundary will be `discarded` depending on the size of the kernel
  - The thickness/size of the foreground object `decreases` 
    - Or simply, the white region decreases in the image
    - Useful for removing small white noise
```python
dilation = cv.dilate(img,kernel,iterations = 1)
```
![alt text](screenshots/erosion.png)

---

## Dilation
- Opposite of erosion
- Pixel elements are considered a `1` 
  - If at least `one pixel` under the kneral is `1`
- So it `increases the white region` in the image or `size of the foreground object` 
- Normally in cases like `noise removal`
  - Erosion is followed by dilation
  - Since erosion removes white noise 
  - But it also shrinks our object

## Original image for reference 

![alt text](screenshots/original.png)

## Opening 
- Another name for `erosion followed by diliation`
- Useful for removing noise 
![alt text](screenshots/opening.png)
```python 
opening = cv.morphologyEx(img, cv.MORPH_OPEN, kernel)
```
---
## Closing 
- Reverse of opening
- Diliation followed by erosion
- Useful for 
  - Closing small holes inside foreground object
  - Small black points on the object
```python
closing = cv.morphologyEx(img, cv.MORPH_CLOSE, kernel)
```
![alt text](screenshots/closing.png)
---

## Morphological Gradient
- The `difference` between diliation and erosion of an image
```python
gradient = cv.morphologyEx(img, cv.MORPH_GRADIENT, kernel)
```
![alt text](screenshots/gradient.png)
---
## Top Hat
- Difference between `input image` and `the opening of the image`
```python
tophat = cv.morphologyEx(img, cv.MORPH_TOPHAT, kernel)
```
---

![alt text](screenshots/tophat.png)
## Black Hat
- Difference between `closing` of theinput image and the input image
```python
blackhat = cv.morphologyEx(img, cv.MORPH_BLACKHAT, kernel)
```
![alt text](screenshots/blackhat.png)