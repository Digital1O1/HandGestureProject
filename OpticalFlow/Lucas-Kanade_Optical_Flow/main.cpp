#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/bgsegm.hpp>

// Global variables for YCrCb trackbar positions
int lowerY = 0, upperY = 255;
int lowerCr = 110, upperCr = 135;
int lowerCb = 100, upperCb = 120;

// Global variables for goodFeaturesToTrack()
int maxCorners = 100;  // Integer value for maxCorners
int qualityLevel = 30; // Integer value (percentage) for quality level of corners
int minDistance = 7;   // Integer value for the minimum distance between corners

using namespace cv;
using namespace std;

// Function to map trackbar integer values to double
double mapTrackbarToDouble(int trackbarValue, double min, double max)
{
    return min + (max - min) * (trackbarValue / 255.0); // Map the trackbar range to the desired double range
}

// Function to create trackbars for YCrCb masking and goodFeaturesToTrack parameters
void createTrackbars()
{
    cv::namedWindow("Trackbars", cv::WINDOW_AUTOSIZE);

    // Create YCrCb trackbars
    cv::createTrackbar("Lower Y (YCrCb)", "Trackbars", &lowerY, 255);
    cv::createTrackbar("Upper Y (YCrCb)", "Trackbars", &upperY, 255);
    cv::createTrackbar("Lower Cr (YCrCb)", "Trackbars", &lowerCr, 255);
    cv::createTrackbar("Upper Cr (YCrCb)", "Trackbars", &upperCr, 255);
    cv::createTrackbar("Lower Cb (YCrCb)", "Trackbars", &lowerCb, 255);
    cv::createTrackbar("Upper Cb (YCrCb)", "Trackbars", &upperCb, 255);

    // Create trackbars for goodFeaturesToTrack parameters
    cv::createTrackbar("Max Corners", "Trackbars", &maxCorners, 500);     // Integer trackbar for maxCorners
    cv::createTrackbar("Quality Level", "Trackbars", &qualityLevel, 100); // Integer trackbar for qualityLevel (0-100)
    cv::createTrackbar("Min Distance", "Trackbars", &minDistance, 50);    // Integer trackbar for minDistance
}

cv::Mat applyYCrCbMask(const cv::Mat &frame)
{
    cv::Mat ycrcbFrame, mask;
    cv::cvtColor(frame, ycrcbFrame, cv::COLOR_BGR2YCrCb);

    // Apply YCrCb masking with trackbar values
    cv::inRange(ycrcbFrame, cv::Scalar(lowerY, lowerCr, lowerCb), cv::Scalar(upperY, upperCr, upperCb), mask);

    // Refine mask
    cv::erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);
    cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);
    cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);

    return mask;
}

bool isPointWithinBounds(const cv::Point2f &point, const cv::Size &size)
{
    return point.x >= 0 && point.x < size.width && point.y >= 0 && point.y < size.height;
}

int main()
{
    cv::VideoCapture cap(0); // Capture video from the webcam
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    // Create the trackbars
    createTrackbars();

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video capture." << std::endl;
        return -1;
    }

    cv::Mat prevGray, gray, frame;

    // Dynamic array of 2D points
    std::vector<cv::Point2f> prevPoints, currPoints;

    // Capture the first frame to initialize previous points
    cap >> frame;

    /* PREPROCESSING SECTION
        Steps to be taken:
            1) Convert input frames to grayscale
            2) Apply noise reduction to avoid detecting false corners due to noise
            3) Mask irrelevant regions like the background so we can isolate the hand
    */
    cv::cvtColor(frame, prevGray, cv::COLOR_BGR2GRAY);

    // Detect initial features to track (e.g., corners or points on hand)
    // Initial values are placeholders. These values will be adjusted dynamically using trackbars.
    cv::goodFeaturesToTrack(prevGray, prevPoints, maxCorners, 0.3, 7);

    // Variable to track the number of frames before re-detecting features
    int frameCount = 0;
    int reDetectionInterval = 30; // Adjust as needed

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        // Convert to grayscale
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Apply YCrCb masking
        cv::Mat gloveMask = applyYCrCbMask(frame);

        // Map trackbar values to their respective double ranges
        double qualityLevel_double = mapTrackbarToDouble(qualityLevel, 0.0, 1.0); // Map to 0.0 to 1.0 range
        double minDistance_double = mapTrackbarToDouble(minDistance, 1.0, 20.0);  // Map to 1.0 to 20.0 range

        // Calculate optical flow
        std::vector<uchar> status;
        std::vector<float> err;

        cv::calcOpticalFlowPyrLK(prevGray, gray, prevPoints, currPoints, status, err);

        // Check for points out of bounds and filter them
        std::vector<cv::Point2f> validPoints;
        for (size_t i = 0; i < currPoints.size(); i++)
        {
            if (status[i] && isPointWithinBounds(currPoints[i], frame.size()))
            {
                validPoints.push_back(currPoints[i]);
            }
        }

        // Update prevPoints with valid points
        prevPoints = validPoints;

        // Draw the motion vectors for valid points
        for (size_t i = 0; i < prevPoints.size(); i++)
        {
            cv::circle(frame, prevPoints[i], 5, cv::Scalar(0, 0, 255), -1);
        }

        // Apply mask to isolate the hand region
        cv::Mat maskedFrame;
        cv::bitwise_and(frame, frame, maskedFrame, gloveMask);

        // Re-detect features periodically
        frameCount++;
        if (frameCount >= reDetectionInterval)
        {
            ;
            // Detect new features in the current frame
            /*
                cv::goodFeaturesToTrack()
                    - ID points of interest to track in an image such as :
                        - Corners
                        - High contrast areas
                    - These points serve as the input for the optical flow algorithm

                qualityLevel
                    - Measures quality of detected corners
                    - Ranges from 0 --> 1
                        - Higher the value; like 0.3, ensures only strong corners are detected

                minDistance
                    - Minimum Eculidean distance between detected corners
                    - Makes sure corners aren't too close to each other to reduce redundancy
            */
            // cv::goodFeaturesToTrack(gray, prevPoints, maxCorners, qualityLevel_double, minDistance_double);
            // frameCount = 0; // Reset frame count
        }

        // Display results
        cv::imshow("Optical Flow - Lucas-Kanade", frame);
        cv::imshow("Glove Mask (YCrCb)", gloveMask);
        cv::imshow("Masked Frame", maskedFrame);

        // Update for next frame
        prevGray = gray.clone();

        if (cv::waitKey(1) == 'q')
            break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
