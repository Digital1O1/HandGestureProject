#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>

void checkCameraProperties(cv::VideoCapture &cap)
{
    std::cout << "Checking supported camera properties...\n";

    struct Property
    {
        int id;
        std::string name;
    };

    Property properties[] = {
        {cv::CAP_PROP_FRAME_WIDTH, "Frame Width"},
        {cv::CAP_PROP_FRAME_HEIGHT, "Frame Height"},
        {cv::CAP_PROP_FPS, "FPS"},
        {cv::CAP_PROP_BRIGHTNESS, "Brightness"},
        {cv::CAP_PROP_CONTRAST, "Contrast"},
        {cv::CAP_PROP_SATURATION, "Saturation"},
        {cv::CAP_PROP_HUE, "Hue"},
        {cv::CAP_PROP_GAIN, "Gain"},
        {cv::CAP_PROP_EXPOSURE, "Exposure"},
        {cv::CAP_PROP_FOCUS, "Focus"},
        {cv::CAP_PROP_AUTOFOCUS, "Autofocus"},
        {cv::CAP_PROP_AUTO_WB, "Auto White Balance"},
        {cv::CAP_PROP_WB_TEMPERATURE, "White Balance Temperature"},
        {cv::CAP_PROP_ZOOM, "Zoom"},
        {cv::CAP_PROP_BUFFERSIZE, "Buffer Size"},
        {cv::CAP_PROP_FOURCC, "FourCC Codec"}};

    for (const auto &prop : properties)
    {
        double value = cap.get(prop.id);
        if (value == -1) // Check if property is unsupported
        {
            std::cout << prop.name << ": Not supported" << std::endl;
        }
        else
        {
            std::cout << prop.name << ": " << value << std::endl;
        }
    }
}

int main()
{
    cv::VideoCapture cap(2, cv::CAP_V4L2); // Use V4L2 for better control on Linux

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    // Set camera properties using system() calls for v4l2-ctl
    std::cout << "Configuring camera settings...\n";
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=white_balance_automatic=0");
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=white_balance_temperature=2000");

    checkCameraProperties(cap); // Verify settings

    // Set resolution manually
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 720);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1280);
    cap.set(cv::CAP_PROP_AUTO_WB, 0);

    cv::Mat frame;
    while (1)
    {
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Blank frame grabbed." << std::endl;
            break;
        }

        cv::imshow("Camera Feed", frame);
        if (cv::waitKey(1) == 'q')
            break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
