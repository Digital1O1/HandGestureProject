#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>

// Global variables for exposure control
int exposureValue = 0; // Initial exposure value (adjust as needed)
int maxExposure = 1;   // Maximum exposure value for the trackbar

// Global variable for the source video (used for the callback)
cv::VideoCapture *cap_ptr = nullptr;

// Trackbar callback to adjust the camera exposure
void setExposureValue(int value, void *userdata)
{
    // Set swtich statement in here later so you can just use one function call for all the parameters you want to use

    // Range: 0 = Manual, 1 = Auto

    std::string exposureCommand = "v4l2-ctl --device=/dev/video2 --set-ctrl=focus_automatic_continuous=" + std::to_string(value);

    int result = system(exposureCommand.c_str());

    std::cout << exposureCommand << std::endl;
    if (result == 0)
    {
        std::cout << "Command executed successfully." << std::endl;
    }
    else if (result == 127)
    {
        std::cout << "The command 'v4l2-ctl' is not found in the PATH." << std::endl;
    }
    else if (result == -1 || result == -2)
    {
        std::cout << "An error occurred while executing the command." << std::endl;
    }
}

int main()
{
    printf("---------------- Commands you can set (User controls) ---------------- \r\n");
    system("v4l2-ctl --device=/dev/video2 --all");
    // Open the camera (device index 2, using V4L2 for Linux)
    cv::VideoCapture cap(2, cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    // For dynamic trackbar callback, store the pointer to cap globally (or pass it via userdata)
    // cap_ptr = &cap;

    // Set a fixed resolution (example)
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    // Optionally, you might want to set initial settings using system() calls here.
    // For example, to disable auto exposure, etc.
    // system("v4l2-ctl --device=/dev/video2 --set-ctrl=auto_exposure=1");

    // Create window to display the camera feed
    cv::namedWindow("Camera Feed", cv::WINDOW_AUTOSIZE);

    cv::createTrackbar("Exposure Value", "Camera Feed", &exposureValue, maxExposure, setExposureValue);

    // Ensure that the current exposure setting is applied at the start.
    setExposureValue(exposureValue, 0);

    cv::Mat frame;
    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Blank frame grabbed." << std::endl;
            break;
        }

        cv::imshow("Camera Feed", frame);
        int key = cv::waitKey(1);
        if (key == 'q')
            break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
