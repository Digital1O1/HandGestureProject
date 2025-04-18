#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/bgsegm.hpp>
#include <chrono>
#include <thread>
#include <atomic>
#include <iomanip>

using namespace cv;
using namespace std;
using namespace std::chrono;

#define USBCAMERA 0

atomic<int> frameCounter(0);
atomic<float> fps(0.0f);
atomic<bool> updateFPS(true);

// YCrCb skin detection thresholds
int lowY = 0, lowCr = 133, lowCb = 77;
int highY = 255, highCr = 173, highCb = 127;

void calculateFPS()
{
    auto lastTime = high_resolution_clock::now();
    while (updateFPS)
    {
        auto currentTime = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(currentTime - lastTime).count();

        if (duration >= 1000) // Update FPS every second
        {
            fps = (frameCounter * 1000.0f) / duration;
            frameCounter = 0;
            lastTime = currentTime;
        }
    }
}

int main()
{

    cv::namedWindow("Hand Detection - YCrCb", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Low Y", "Hand Detection - YCrCb", &lowY, 255);
    cv::createTrackbar("High Y", "Hand Detection - YCrCb", &highY, 255);
    cv::createTrackbar("Low Cr", "Hand Detection - YCrCb", &lowCr, 255);
    cv::createTrackbar("High Cr", "Hand Detection - YCrCb", &highCr, 255);
    cv::createTrackbar("Low Cb", "Hand Detection - YCrCb", &lowCb, 255);
    cv::createTrackbar("High Cb", "Hand Detection - YCrCb", &highCb, 255);

    Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorMOG2();
    VideoCapture usbCamera(USBCAMERA);

    if (!usbCamera.isOpened())
    {
        cerr << "Can't open camera\n\n";
        return -1;
    }

    usbCamera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    usbCamera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    Mat frame, ycrcbImage;
    Mat skinMaskYCrCbOutput, fgMask, combinedMask;

    // Define skin color ranges for YCrCb
    Scalar ycrcbLower(0, 133, 77);
    Scalar ycrcbUpper(255, 173, 127);

    int screen_width = 1920;
    int screen_height = 1080;
    int window_width = 640;
    int window_height = 480;
    int posX = (screen_width - window_width) / 2;
    int posY = (screen_height - window_height) / 2;

    namedWindow("Hand Gesture Detection");
    moveWindow("Hand Gesture Detection", posX, posY);

    thread fpsThread(calculateFPS);

    while (true)
    {
        usbCamera >> frame;

        if (frame.empty())
        {
            cerr << "Error: Frame is empty" << endl;
            break;
        }

        cvtColor(frame, ycrcbImage, COLOR_BGR2YCrCb);

        // Apply skin color segmentation in YCrCb
        // inRange(ycrcbImage, ycrcbLower, ycrcbUpper, skinMaskYCrCbOutput);

        // Define lower and upper bounds for skin color in YCrCb color space
        cv::Scalar lower(lowY, lowCr, lowCb);
        cv::Scalar upper(highY, highCr, highCb);

        // Apply threshold to isolate skin regions
        cv::inRange(ycrcbImage, lower, upper, skinMaskYCrCbOutput);

        // Apply Background Subtraction on the original frame
        pBackSub->apply(frame, fgMask);

        // Combine the skin mask with the foreground mask
        // I didn't do this on my first attempt
        // Was used so we could combine two masks in a way that only keeps the regoins where both masks have non-zero/active pixels
        bitwise_and(fgMask, skinMaskYCrCbOutput, combinedMask);

        // Display FPS on the frame
        frameCounter++;
        stringstream ss;
        ss << "FPS: " << fixed << setprecision(2) << fps;
        std::cout << "FPS : " << fps << std::endl;
        putText(combinedMask, ss.str(), Point(15, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        putText(ycrcbImage, ss.str(), Point(15, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

        // Show the combined mask
        imshow("Hand Gesture Detection", combinedMask);
        imshow("ycrcbImage", ycrcbImage);

        if (waitKey(1) == 'q')
        {
            break;
        }
    }

    updateFPS = false;
    fpsThread.join();

    return 0;
}
