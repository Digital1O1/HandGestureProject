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

#define USBCAMERA 2

// std::atomic<Type> variableName(initialValue);
atomic<int> frameCounter(0);
atomic<float> fps(0.0f);
atomic<bool> updateFPS(true);

void calculateFPS()
{
    auto lastTime = high_resolution_clock::now();
    while (updateFPS)
    {
        // this_thread::sleep_for(milliseconds(100)); // Update every 100ms
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
    // Create a Background Subtractor object pointer
    // Or you can use createBackgroundSubtractorKNN()
    Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorMOG2();
    Ptr<BackgroundSubtractor> pBackSub2 = cv::bgsegm::createBackgroundSubtractorGMG();
    VideoCapture usbCamera(USBCAMERA);

    if (!usbCamera.isOpened())
    {
        cerr << "Can't open camera\n\n";
        return -1;
    }

    usbCamera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    usbCamera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    Mat frame, hsvImage, ycrcbImage;
    Mat skinMaskHSVOutput, skinMaskYCrCbOutput;
    Mat combinedImage, fgMask, fgMask2;

    // Define skin color ranges for HSV
    // Scalar values represent HSV values
    Scalar hsvLower(0, 20, 70);
    Scalar hsvUpper(20, 255, 255);

    // Define skin color ranges for YCrCb
    // Same goes with these scalar values with Y | Cr | Cb
    Scalar ycrcbLower(0, 133, 77);
    Scalar ycrcbUpper(255, 173, 127);

    // Get screen resolution
    int screen_width = 1920;  // Replace with actual screen width
    int screen_height = 1080; // Replace with actual screen height

    // Calculate sizes for combined image
    int window_width = 640 * 3; // Width of frame + two masks
    int window_height = 480;    // Height of the frame (same for masks)

    // Calculate positions to center the window
    int posX = (screen_width - window_width) / 2;
    int posY = (screen_height - window_height) / 2;

    namedWindow("skinMaskYCrCb3");
    // void cv::moveWindow(const cv::String &winname, int x, int y)
    moveWindow("skinMaskYCrCb3", posX, posY);

    // Start the FPS calculation thread
    thread fpsThread(calculateFPS);

    while (true)
    {
        auto frameStartTime = high_resolution_clock::now();
        usbCamera >> frame;

        if (frame.empty())
        {
            cerr << "Error: Frame is empty" << endl;
            break;
        }

        // cvtColor(frame, hsvImage, COLOR_BGR2HSV);
        cvtColor(frame, ycrcbImage, COLOR_BGR2YCrCb);

        // void cv::inRange(cv::InputArray src, cv::InputArray lowerb, cv::InputArray upperb, cv::OutputArray dst)
        // Apply skin color segmentation in HSV
        // inRange(hsvImage, hsvLower, hsvUpper, skinMaskHSVOutput);

        // Apply skin color segmentation in YCrCb

        // void cv::inRange(cv::InputArray src, cv::InputArray lowerb, cv::InputArray upperb, cv::OutputArray dst)
        inRange(ycrcbImage, ycrcbLower, ycrcbUpper, skinMaskYCrCbOutput);

        // Convert single-channel masks to 3-channel images for concatenation
        Mat skinMaskHSV3, skinMaskYCrCb3;
        // cvtColor(skinMaskHSVOutput, skinMaskHSV3, COLOR_GRAY2BGR);
        cvtColor(skinMaskYCrCbOutput, skinMaskYCrCb3, COLOR_GRAY2BGR);

        // Update the background model
        // pBackSub->apply(frame, fgMask);
        pBackSub->apply(skinMaskYCrCb3, fgMask2);

        // Increment frame counter
        frameCounter++;

        // Display FPS on the frame
        stringstream ss;
        ss << "FPS: " << fixed << setprecision(2) << fps;
        string fpsString = ss.str();
        cout << "FPS : " << fpsString << std::endl;

        rectangle(frame, Point(10, 2), Point(100, 20), Scalar(255, 255, 255), -1);
        putText(frame, fpsString.c_str(), Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));

        rectangle(frame, Point(10, 2), Point(100, 20), Scalar(255, 255, 255), -1);
        putText(frame, fpsString.c_str(), Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));

        // Show the current frame and the fg masks
        // imshow("Frame", frame);
        // imshow("FG Mask", fgMask);
        imshow("skinMaskYCrCb3", fgMask2);
        if (waitKey(1) == 'q')
        {
            break;
        }
    }

    // Stop the FPS calculation thread
    updateFPS = false;
    fpsThread.join();

    return 0;
}
