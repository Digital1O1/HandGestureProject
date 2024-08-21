#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::VideoCapture cap(2); // Open the default camera

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera" << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    cv::Mat frame, grayFrame, blurredFrame, thresholdedFrame;
    while (true)
    {
        cap >> frame;

        if (frame.empty())
        {
            std::cerr << "Error: Frame is empty" << std::endl;
            // break;
        }

        // Convert to grayscale
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        // Apply Gaussian blur
        // void cv::GaussianBlur(cv::InputArray src, cv::OutputArray dst, cv::Size ksize, double sigmaX, double sigmaY = (0.0), int borderType = 4)
        cv::GaussianBlur(grayFrame, blurredFrame, cv::Size(5, 5), 0);

        // Threshold the image
        // double cv::threshold(cv::InputArray src, cv::OutputArray dst, double thresh, double maxval, int type)
        cv::threshold(blurredFrame, thresholdedFrame, 60, 255, cv::THRESH_BINARY);

        // Find contours
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(thresholdedFrame, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        // Draw contours
        cv::Mat contourOutput = frame.clone();
        cv::drawContours(contourOutput, contours, -1, cv::Scalar(0, 255, 0), 2);

        // Display the frame
        cv::imshow("grayFrame", grayFrame);
        cv::moveWindow("grayFrame", 100, 300);
        cv::imshow("ThresholdedFrame", thresholdedFrame);
        cv::moveWindow("ThresholdedFrame", 900, 300);

        cv::imshow("Contours", contourOutput);
        cv::moveWindow("Contours", 1200, 300);

        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
