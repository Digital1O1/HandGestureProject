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

    // Use sysfs rebind to 'reset' the webcamera
    // Use lsusb to find webcamera bus ID and replace it with '1-1' as shown in the example below
    cv::VideoCapture cap(2, cv::CAP_V4L2); // Use V4L2 for better control on Linux

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    std::cout << "Configuring camera settings...\n";

    // Disable auto exposure (set to Manual Mode)
    // Range: 1 = Manual mode | 3 = Aperature Priority Mode
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=auto_exposure=1");

    // Disable auto white balance
    // Range: 0 = Manual, 1 = Auto
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=white_balance_automatic=0");

    // Disable auto focus (use a fixed focus instead)
    // Range: 0 = Manual, 1 = Auto
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_automatic_continuous=0");

    // Set focus to a fixed value
    // Range: min=-36000, max=36000, step=3600, default=0
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_absolute=0");

    // Disable exposure dynamic framerate
    // Range: 0 = Disabled, 1 = Enabled
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=exposure_dynamic_framerate=0");

    // Set fixed white balance temperature
    // Range: min=2800K, max=6500K, step=100K, default=4000K
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=white_balance_temperature=4000");

    // Set exposure time
    // Range: min=1, max=10000, step=1, default=500
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=exposure_time_absolute=500");

    // Set brightness
    // Range: min=0, max=255, step=1, default=128
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=brightness=128");

    // Set contrast
    // Range: min=0, max=255, step=1, default=128
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=contrast=128");

    // Set saturation
    // Range: min=0, max=255, step=1, default=128
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=saturation=128");

    // Set sharpness
    // Range: min=0, max=255, step=1, default=128
    system("v4l2-ctl --device=/dev/video2 --set-ctrl=sharpness=128");

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

        int input = cv::waitKey(1);

        switch (input)
        {
        case 'q':
        {
            printf("Quitting program now...\r\n");
            cap.release();
            cv::destroyAllWindows();
            exit(1);
            break;
        }
        case ' ':
        {
            std::cout << "\n--- Camera Settings ---\n";
            system("v4l2-ctl --device=/dev/video2 --get-ctrl=auto_exposure");
            system("v4l2-ctl --device=/dev/video2 --get-ctrl=white_balance_automatic");
            system("v4l2-ctl --device=/dev/video2 --get-ctrl=focus_absolute");
        }
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
// v4l2-ctl --device=/dev/video2 --all
