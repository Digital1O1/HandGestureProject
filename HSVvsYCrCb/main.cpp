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

int main()
{
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
    Mat combinedImage;

    /*
        Typical values to use

        HSV
            - Hue
                - Ranges from 0 50 degrees for most skin tones
            - Saturation
                - Set to higher value to avoid detecting non-skin areas
                - Example : 0.2 to 0.8
            - Value
                - Can vary based on lighting
                - Example 0.4 - 1.0
        YCrCb
            - Cr
                - Typically around 133 and 173
            - Cb
                - Typically between 77 and 127
    */

    // Define skin color ranges for HSV
    // Scalar values represent HSV values
    Scalar hsvLower(0, 20, 70);
    Scalar hsvUpper(20, 255, 255);

    // Define skin color ranges for YCrCb
    // Same goes with these scalar values with Y | Cr | Cb
    Scalar ycrcbLower(0, 133, 77);
    Scalar ycrcbUpper(255, 173, 127);

    // Get screen resolution
    int screen_width = 1920;
    int screen_height = 1080;

    // Calculate sizes for combined image
    int window_width = 640 * 3; // Width of frame + two masks
    int window_height = 480;    // Height of the frame (same for masks)

    // Calculate positions to center the window
    int posX = (screen_width - window_width) / 2;
    int posY = (screen_height - window_height) / 2;

    namedWindow("Frame || skinMaskHSV3 || skinMaskYCrCb3");
    // void cv::moveWindow(const cv::String &winname, int x, int y)
    moveWindow("Frame || skinMaskHSV3 || skinMaskYCrCb3", posX, posY);

    while (true)
    {
        auto frameStartTime = high_resolution_clock::now();
        usbCamera >> frame;

        if (frame.empty())
        {
            cerr << "Error: Frame is empty" << endl;
            break;
        }

        // void cv::cvtColor(cv::InputArray src, cv::OutputArray dst, int code, int dstCn = 0)
        // Convert to HSV and YCrCb color spaces
        cvtColor(frame, hsvImage, COLOR_BGR2HSV);
        cvtColor(frame, ycrcbImage, COLOR_BGR2YCrCb);

        // void cv::inRange(cv::InputArray src, cv::InputArray lowerb, cv::InputArray upperb, cv::OutputArray dst)
        // Apply skin color segmentation in HSV
        inRange(hsvImage, hsvLower, hsvUpper, skinMaskHSVOutput);

        // Apply skin color segmentation in YCrCb
        inRange(ycrcbImage, ycrcbLower, ycrcbUpper, skinMaskYCrCbOutput);

        // Convert single-channel masks to 3-channel images for concatenation
        Mat skinMaskHSV3, skinMaskYCrCb3;
        cvtColor(skinMaskHSVOutput, skinMaskHSV3, COLOR_GRAY2BGR);
        cvtColor(skinMaskYCrCbOutput, skinMaskYCrCb3, COLOR_GRAY2BGR);

        // Combine the images horizontally
        hconcat(frame, skinMaskHSV3, combinedImage);           // This combinds the 'raw' video feed along with skinMaskHSV3 into combinedImage
        hconcat(combinedImage, skinMaskYCrCb3, combinedImage); // This takes combinedImage and adds skinMaskYCrCb3 and I guess adds it onto combinedImage again

        // Show the combined image
        imshow("Frame || skinMaskHSV3 || skinMaskYCrCb3", combinedImage);

        if (waitKey(1) == 'q')
        {
            break;
        }
    }

    return 0;
}
