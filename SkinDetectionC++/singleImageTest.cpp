#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#define USBCAMERA 2

using namespace cv;
using namespace std;

int main()
{
    Mat frame, grayFrame;

    frame = cv::imread("/home/digital101/LinuxCodingFolder/HandGestureProject/SkinDetectionC++/TestImages/protrait.jpg", cv::IMREAD_COLOR);

    cv::resize(frame, frame, cv::Size(640, 480));

    if (frame.empty())
    {
        std::cout << "Can't read image" << std::endl;
        exit(1);
    }

    // HSV and YCbCr masks, kernels
    Mat hsvMask, hsvImage;
    Mat yCbCrMask, yCbCrImage;
    Mat global_mask;
    Mat HSV_result, YCrCb_result, global_result;
    Mat finalImage;

    int lowerH = 0, lowerS = 15, lowerV = 0;
    int upperH = 17, upperS = 170, upperV = 255;
    Scalar lower_hsv(lowerH, lowerS, lowerV);
    Scalar upper_hsv(upperH, upperS, upperV);

    cv::Mat kernel_3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat kernel_4x4 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));

    // Convert to HSV and apply HSV mask
    cvtColor(frame, hsvImage, COLOR_BGR2HSV);
    inRange(hsvImage, lower_hsv, upper_hsv, hsvMask);
    cv::morphologyEx(hsvMask, hsvMask, cv::MORPH_OPEN, kernel_3x3);

    // Convert to YCbCr and apply YCbCr mask
    cv::cvtColor(frame, yCbCrImage, cv::COLOR_BGR2YCrCb);
    cv::Scalar lower_YCrCb(0, 135, 85);
    cv::Scalar upper_YCrCb(255, 180, 135);
    cv::inRange(yCbCrImage, lower_YCrCb, upper_YCrCb, yCbCrMask);
    cv::morphologyEx(yCbCrMask, yCbCrMask, cv::MORPH_OPEN, kernel_3x3);

    // Merge the YCbCr and HSV masks using bitwise AND
    cv::bitwise_and(yCbCrMask, hsvMask, global_mask);

    // Apply a median blur to the global mask
    cv::medianBlur(global_mask, global_mask, 3);

    // Apply another opening morphological transformation to the global mask
    cv::morphologyEx(global_mask, global_mask, cv::MORPH_OPEN, kernel_4x4);

    // Invert the masks
    cv::bitwise_not(hsvMask, HSV_result);
    cv::bitwise_not(yCbCrMask, YCrCb_result);
    cv::bitwise_not(global_mask, global_result);

    // Display results
    imshow("Original Frame", frame);
    // imshow("HSV Mask", hsvMask);
    // imshow("YCbCr Mask", yCbCrMask);
    // imshow("Global Mask", global_mask);
    imshow("HSV Result", HSV_result);
    imshow("YCbCr Result", YCrCb_result);
    imshow("Global Result", global_result);

    cv::waitKey(0);

    return 0;
}
