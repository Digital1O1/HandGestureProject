#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

// HSV skin detection thresholds
int lowH = 0, lowS = 48, lowV = 80;
int highH = 20, highS = 255, highV = 255;

// YCrCb skin detection thresholds
int lowY = 0, lowCr = 133, lowCb = 77;
int highY = 255, highCr = 173, highCb = 127;

cv::Mat filterSkinHSV(cv::Mat &frame)
{
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower(lowH, lowS, lowV);
    cv::Scalar upper(highH, highS, highV);

    cv::Mat skinMask;
    cv::inRange(hsv, lower, upper, skinMask);

    // cv::Point(-1,-1) indicates the anchor is set to the center of the default 3x3 kernel
    // Setting iterations to 2 applies the erosion operation twice, which can help remove more noise or refine edges further
    cv::erode(skinMask, skinMask, cv::Mat(), cv::Point(-1, -1), 2);
    cv::dilate(skinMask, skinMask, cv::Mat(), cv::Point(-1, -1), 2);

    cv::GaussianBlur(skinMask, skinMask, cv::Size(5, 5), 0);

    return skinMask;
}

cv::Mat filterSkinYCrCb(cv::Mat &frame)
{
    cv::Mat ycrcb;
    cv::cvtColor(frame, ycrcb, cv::COLOR_BGR2YCrCb);

    cv::Scalar lower(lowY, lowCr, lowCb);
    cv::Scalar upper(highY, highCr, highCb);

    cv::Mat skinMask;
    cv::inRange(ycrcb, lower, upper, skinMask);

    cv::erode(skinMask, skinMask, cv::Mat(), cv::Point(-1, -1), 2);
    cv::dilate(skinMask, skinMask, cv::Mat(), cv::Point(-1, -1), 2);

    cv::GaussianBlur(skinMask, skinMask, cv::Size(5, 5), 0);

    return skinMask;
}

int main()
{
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video stream!" << std::endl;
        return -1;
    }

    cv::namedWindow("Hand Detection - HSV", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Low H", "Hand Detection - HSV", &lowH, 180);
    cv::createTrackbar("High H", "Hand Detection - HSV", &highH, 180);
    cv::createTrackbar("Low S", "Hand Detection - HSV", &lowS, 255);
    cv::createTrackbar("High S", "Hand Detection - HSV", &highS, 255);
    cv::createTrackbar("Low V", "Hand Detection - HSV", &lowV, 255);
    cv::createTrackbar("High V", "Hand Detection - HSV", &highV, 255);

    cv::namedWindow("Hand Detection - YCrCb", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Low Y", "Hand Detection - YCrCb", &lowY, 255);
    cv::createTrackbar("High Y", "Hand Detection - YCrCb", &highY, 255);
    cv::createTrackbar("Low Cr", "Hand Detection - YCrCb", &lowCr, 255);
    cv::createTrackbar("High Cr", "Hand Detection - YCrCb", &highCr, 255);
    cv::createTrackbar("Low Cb", "Hand Detection - YCrCb", &lowCb, 255);
    cv::createTrackbar("High Cb", "Hand Detection - YCrCb", &highCb, 255);

    while (true)
    {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Could not capture frame!" << std::endl;
            break;
        }

        cv::Mat skinMaskHSV = filterSkinHSV(frame);
        cv::Mat skinMaskYCrCb = filterSkinYCrCb(frame);

        std::vector<std::vector<cv::Point>> contoursHSV;
        cv::findContours(skinMaskHSV, contoursHSV, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        cv::Mat contourOutputHSV = frame.clone();
        cv::drawContours(contourOutputHSV, contoursHSV, -1, cv::Scalar(0, 255, 0), 2); // Green contours for HSV

        std::vector<std::vector<cv::Point>> contoursYCrCb;
        cv::findContours(skinMaskYCrCb, contoursYCrCb, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        cv::Mat contourOutputYCrCb = frame.clone();
        cv::drawContours(contourOutputYCrCb, contoursYCrCb, -1, cv::Scalar(255, 0, 0), 2); // Blue contours for YCrCb

        // Show both detection results
        // cv::imshow("Hand Detection - HSV", contourOutputHSV);
        cv::imshow("Hand Detection - YCrCb", contourOutputYCrCb);

        // Show the skin masks for debugging purposes
        // cv::imshow("Skin Mask - HSV", skinMaskHSV);
        // cv::imshow("Skin Mask - YCrCb", skinMaskYCrCb);

        // Break the loop if 'q' is pressed
        char key = (char)cv::waitKey(1);
        if (key == 'q')
        {
            break;
        }
    }

    // Release the camera and close windows
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
