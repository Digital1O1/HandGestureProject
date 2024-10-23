#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <chrono>
#include <thread>
#include <atomic>
#include <iomanip>
using namespace cv;
using namespace std;
using namespace std::chrono;

#define USBCAMERA 0

// std::atomic<Type> variableName(initialValue);
atomic<int> frameCounter(0);
atomic<float> fps(0.0f);
atomic<bool> updateFPS(true);

void calculateFPS()
{
    auto lastTime = high_resolution_clock::now();
    while (updateFPS)
    {
        auto currentTime = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(currentTime - lastTime).count();

        if (duration >= 1000) 
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
    Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorMOG2();

    std::string inputVideo = "/home/digital101/LinuxCodingFolder/HandGestureProject/BackgroundSubtraction/matrixBGTesting.mp4";
    std::string outputVideo = "matrixTesting_640x480.mp4";

    VideoCapture usbCamera(inputVideo);

    if (!usbCamera.isOpened())
    {
        cerr << "Can't open MP4 file...\n\n";
        return -1;
    }

    int frame_width = static_cast<int>(usbCamera.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(usbCamera.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps_original = usbCamera.get(cv::CAP_PROP_FPS);

    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v'); // Codec for MP4 format
    cv::VideoWriter out(outputVideo, fourcc, fps_original, cv::Size(640, 480));

    Mat frame, fgMask, resized_frame;

    // Get screen resolution
    int screen_width = 1920;  // Replace with actual screen width
    int screen_height = 1080; // Replace with actual screen height

    // Calculate positions to center the windows
    int window_width = 640;
    int window_height = 480;

    int posX1 = (screen_width - 2 * window_width) / 2;
    int posY1 = (screen_height - window_height) / 2;

    int posX2 = posX1 + window_width;
    int posY2 = posY1;

    namedWindow("Frame");
    namedWindow("FG Mask");

    moveWindow("Frame", posX1, posY1);
    moveWindow("FG Mask", posX2, posY2);

    // Start the FPS calculation thread
    thread fpsThread(calculateFPS);

    while (true)
    {
        usbCamera >> frame;

        if (frame.empty())
        {
            cerr << "Error: Frame is empty" << endl;
            break;
        }

        // Resize the frame to 640x480
        cv::resize(frame, resized_frame, cv::Size(640, 480));

        // Update the background model with the resized frame
        pBackSub->apply(resized_frame, fgMask);

        // Increment frame counter
        frameCounter++;

        // Display FPS on the frame
        stringstream ss;
        ss << "FPS: " << fixed << setprecision(2) << fps;
        string fpsString = ss.str();

        rectangle(resized_frame, Point(10, 2), Point(100, 20), Scalar(255, 255, 255), -1);
        putText(resized_frame, fpsString.c_str(), Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));

        // Write the resized frame to the output video
        out.write(resized_frame);

        // Show the current frame and the fg mask
        imshow("Frame", resized_frame);
        imshow("FG Mask", fgMask);

        if (waitKey(1) == 'q')
        {
            break;
        }
    }

    // Stop the FPS calculation thread
    updateFPS = false;
    fpsThread.join();

    // Release resources
    usbCamera.release();
    out.release();

    return 0;
}
