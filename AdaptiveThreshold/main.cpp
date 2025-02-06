#include <opencv2/opencv.hpp>
#include <iostream>

// YCrCb skin detection thresholds
int lowY = 0, lowCr = 133, lowCb = 77;
int highY = 255, highCr = 173, highCb = 127;
int threshold1 = 0;
int threshold2 = 0;

// Function to filter skin regions in YCrCb color space
cv::Mat filterSkinYCrCb(cv::Mat &frame)
{
    cv::Mat ycrcb, skinMask;
    cv::cvtColor(frame, ycrcb, cv::COLOR_BGR2YCrCb); // Convert frame to YCrCb color space

    // Define lower and upper bounds for skin color in YCrCb color space
    cv::Scalar lower(lowY, lowCr, lowCb);
    cv::Scalar upper(highY, highCr, highCb);

    // Apply threshold to isolate skin regions
    cv::inRange(ycrcb, lower, upper, skinMask);

    // Morphological operations to reduce noise
    cv::erode(skinMask, skinMask, cv::Mat(), cv::Point(-1, -1), 2);
    cv::dilate(skinMask, skinMask, cv::Mat(), cv::Point(-1, -1), 2);

    // Optional: Smooth out edges to help contour detection
    cv::GaussianBlur(skinMask, skinMask, cv::Size(5, 5), 0);

    return skinMask;
}

int main()
{
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    // Create a window with trackbars for adjusting skin detection thresholds
    cv::namedWindow("Hand Detection - YCrCb", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Low Y", "Hand Detection - YCrCb", &lowY, 255);
    cv::createTrackbar("High Y", "Hand Detection - YCrCb", &highY, 255);
    cv::createTrackbar("Low Cr", "Hand Detection - YCrCb", &lowCr, 255);
    cv::createTrackbar("High Cr", "Hand Detection - YCrCb", &highCr, 255);
    cv::createTrackbar("Low Cb", "Hand Detection - YCrCb", &lowCb, 255);
    cv::createTrackbar("High Cb", "Hand Detection - YCrCb", &highCb, 255);
    cv::createTrackbar("threshold 1", "Hand Detection - YCrCb", &threshold1, 300);
    cv::createTrackbar("threshold 2", "Hand Detection - YCrCb", &threshold2, 300);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video capture." << std::endl;
        return -1;
    }

    while (true)
    {
        cv::Mat frame, grayFrame, adaptiveThresh, blurredFrame, edges;

        // Capture frame-by-frame
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Empty frame captured." << std::endl;
            break;
        }

        // Perform skin detection in YCrCb color space
        cv::Mat skinMaskYCrCb = filterSkinYCrCb(frame);

        // Convert frame to grayscale
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        // Option 1: Apply Gaussian Blur before adaptive thresholding
        // cv::GaussianBlur(grayFrame, blurredFrame, cv::Size(5, 5), 1.5);
        // cv::adaptiveThreshold(blurredFrame, adaptiveThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
        //                       cv::THRESH_BINARY, 11, 2);

        // Option 2: Skip Gaussian Blur (uncomment to try)
        // void cv::adaptiveThreshold(InputArray src, OutputArray dst, double maxValue, int adaptiveMethod, int thresholdType, int blockSize, double C)
        // Check codingNotes.md for more detail about how to determine the right blockSize
        // Applies threshold dynamically for each pixel based on it's neighborhood which makes it best for images with varying lighting conditions
        cv::adaptiveThreshold(grayFrame, adaptiveThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                              cv::THRESH_BINARY, 11, 2);

        /*
            Keep in mind that:
                - GaussianBlur() is typically used before a Canny edge detector to smooth out an image by reducing noise and minor details that could significantly improve edge detection.
                    - It can help with high noise images since blurring can reduce noise and prevent small variations from affecting the adaptive thresholding.
                    - Or if you're wanting smoother edge continuity:
                        - Blurring can help with smoothing out minor pixel variations.
                        - Leading to cleaner contours post-threshold that could improve the Canny edge detector.
                - But if your images are clean:
                    - You can skip out on blurring.
                    - And since we're dealing with a lot of light in a surgical setting, we can totally skip this step.
        */

        // Detect contours on the skin mask
        std::vector<std::vector<cv::Point>> contoursYCrCb;
        cv::findContours(skinMaskYCrCb, contoursYCrCb, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Draw contours on a copy of the original frame
        cv::Mat contourOutputYCrCb = frame.clone();
        cv::drawContours(contourOutputYCrCb, contoursYCrCb, -1, cv::Scalar(255, 0, 0), 2); // Blue

        // Apply Canny Edge Detection on the adaptive thresholded image
        cv::Canny(adaptiveThresh, edges, threshold1, threshold2);

        // Display the results
        // cv::imshow("Original Frame", frame);
        cv::imshow("Adaptive Thresholding", adaptiveThresh);
        cv::imshow("Edges Detected", edges);
        cv::imshow("Hand Detection - YCrCb", contourOutputYCrCb);

        // Press 'q' to exit the loop
        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
