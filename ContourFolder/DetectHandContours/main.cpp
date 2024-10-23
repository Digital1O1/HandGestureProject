#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int lowH = 0, lowS = 48, lowV = 80;
int highH = 20, highS = 255, highV = 255;

cv::Mat filterSkin(cv::Mat &frame)
{
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower(lowH, lowS, lowV);
    cv::Scalar upper(highH, highS, highV);

    cv::Mat skinMask;
    cv::inRange(hsv, lower, upper, skinMask);

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

    cv::namedWindow("Hand Detection", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Low H", "Hand Detection", &lowH, 180);
    cv::createTrackbar("High H", "Hand Detection", &highH, 180);
    cv::createTrackbar("Low S", "Hand Detection", &lowS, 255);
    cv::createTrackbar("High S", "Hand Detection", &highS, 255);
    cv::createTrackbar("Low V", "Hand Detection", &lowV, 255);
    cv::createTrackbar("High V", "Hand Detection", &highV, 255);

    /*
        values to use

        Low h 0
        High H 13
        Low s 58
        High s 183
        Low v 78
        High v 184q
    */
    while (true)
    {
        cv::Mat frame;
        cap >> frame; // Capture each frame
        if (frame.empty())
        {
            std::cerr << "Error: Could not capture frame!" << std::endl;
            break;
        }

        cv::Mat skinMask = filterSkin(frame);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(skinMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        cv::Mat contourOutput = frame.clone();
        cv::drawContours(contourOutput, contours, -1, cv::Scalar(0, 255, 0), 2); // Green contours

        // Show the original frame with contours
        cv::imshow("Hand Detection", contourOutput);

        // Show the skin mask for debugging purposes
        cv::imshow("Skin Mask", skinMask);

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
