#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

// Function to apply adaptive thresholding
cv::Mat applyAdaptiveThreshold(const cv::Mat &grayFrame)
{
    cv::Mat thresholded;
    cv::adaptiveThreshold(grayFrame, thresholded, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY, 11, 2);
    return thresholded;
}

// Function to compute and display optical flow
void computeOpticalFlow(cv::Mat &prevGray, cv::Mat &gray, const cv::Mat &mask)
{
    std::vector<cv::Point2f> prevPoints, currPoints;
    cv::goodFeaturesToTrack(prevGray, prevPoints, 100, 0.3, 7, mask);

    if (!prevPoints.empty())
    {
        std::vector<uchar> status;
        std::vector<float> err;
        cv::calcOpticalFlowPyrLK(prevGray, gray, prevPoints, currPoints, status, err);

        // Draw motion vectors
        for (size_t i = 0; i < currPoints.size(); i++)
        {
            if (status[i])
            {
                cv::line(mask, prevPoints[i], currPoints[i], cv::Scalar(0, 255, 0), 2);
                cv::circle(mask, currPoints[i], 5, cv::Scalar(0, 0, 255), -1);
            }
        }
    }
}

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video capture." << std::endl;
        return -1;
    }

    cv::Mat frame, gray, prevGray, adaptiveThresh;
    cap >> frame;
    cv::cvtColor(frame, prevGray, cv::COLOR_BGR2GRAY); // Initialize prevGray

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        // Convert frame to grayscale
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Apply adaptive thresholding to isolate the hand
        adaptiveThresh = applyAdaptiveThreshold(gray);

        // Compute optical flow using the thresholded mask
        cv::Mat mask = adaptiveThresh.clone();
        computeOpticalFlow(prevGray, gray, mask);

        // Display results
        cv::imshow("Original Frame", frame);
        cv::imshow("Adaptive Thresholding", adaptiveThresh);
        cv::imshow("Optical Flow", mask);

        prevGray = gray.clone();

        if (cv::waitKey(30) == 'q')
            break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
