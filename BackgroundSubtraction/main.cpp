#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
using namespace cv;
using namespace std;
#define USBCAMERA 2
int main()
{
    // create Background Subtractor objects
    Ptr<BackgroundSubtractor> pBackSub1;
    Ptr<BackgroundSubtractor> pBackSub2;

    pBackSub1 = createBackgroundSubtractorMOG2();
    pBackSub2 = createBackgroundSubtractorKNN();

    VideoCapture usbCamera(USBCAMERA);

    if (!usbCamera.isOpened())
    {
        std::cerr << "Can't open camera\n\n";
        return -1;
    }

    usbCamera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    usbCamera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    Mat frame, fgMask;

    while (true)
    {
        usbCamera >> frame;

        if (frame.empty())
        {
            std::cerr << "Error: Frame is empty" << std::endl;
            // break;
        }

        // update the background model
        pBackSub1->apply(frame, fgMask);
        // get the frame number and write it on the current frame
        rectangle(frame, cv::Point(10, 2), cv::Point(100, 20),
                  cv::Scalar(255, 255, 255), -1);
                  
        stringstream ss;
        ss << usbCamera.get(CAP_PROP_POS_FRAMES);

        string frameNumberString = ss.str();

        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));

        // show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask", fgMask);
        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    return 0;
}