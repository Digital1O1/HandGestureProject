#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>

// Globals for trackbars
int exposureValue = 3;    // Range: 3–2047 (for exposure_time_absolute)
int focusValue = 30;      // Range: 0–250
int autofocusEnabled = 1; // 1 = on, 0 = off
int setAperature = 3;
int setAutoExposureValue = 3;
int setPanAbsoluteValue = 0;

// Additional globals for recommended controls
int brightnessValue = 128;
int contrastValue = 128;
int saturationValue = 128;
int gainValue = 255;
int sharpnessValue = 128;
int backlightCompensation = 0;
int whiteBalanceTemperature = 4000;
int whiteBalanceAuto = 1;

void runCommand(const std::string &command)
{
    int result = system(command.c_str());
    std::cout << "\n[CMD] " << command << std::endl;

    if (result == 0)
        std::cout << command << " executed successfully.\n";
    else if (result == 127)
        std::cout << "❌ 'v4l2-ctl' not found in PATH.\n";
    else
        std::cout << "⚠️ Error executing command. Return code: " << result << "\n";
}

// Callback functions to be executed when trackbar values change

void setAutoExposure(int value, void *)
{
    // auto_exposure 0x009a0901 (menu)   : min=0 max=3 default=3 value=3 (Aperture       Priority Mode)
    //  1: Manual Mode
    //  3: Aperture Priority Mode
    if (value == 0)
        runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=auto_exposure=1");
    if (value == 1)
        runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=auto_exposure=3");
}

void setExposureTime(int value, void *)
{
    // exposure_time_absolute 0x009a0902 (int)    : min=3 max=2047 step=1 default=250 value=333 flags=inactive
    //  runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=exposure_auto=1"); // Set manual mode
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=exposure_time_absolute=" + std::to_string(value));
}

void onFocusChange(int value, void *)
{
    // runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_auto=0"); // Disable autofocus
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_absolute=" + std::to_string(value));
}

void onAutofocusToggle(int value, void *)
{
    if (value == 0)
        runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_automatic_continuous=0");
    if (value == 1)
        runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_automatic_continuous=1");
}

// Additional recommended control callbacks

void setBrightness(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=brightness=" + std::to_string(value));
}

void setContrast(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=contrast=" + std::to_string(value));
}

void setSaturation(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=saturation=" + std::to_string(value));
}

void setGain(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=gain=" + std::to_string(value));
}

void setSharpness(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=sharpness=" + std::to_string(value));
}

void setBacklightComp(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=backlight_compensation=" + std::to_string(value));
}

void setWhiteBalanceTemperature(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=white_balance_temperature=" + std::to_string(value));
}

void setWhiteBalanceAuto(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=white_balance_automatic=" + std::to_string(value));
}

// Main app
int main()
{
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // stringstream since it accepts complex values and foramts them into strings and is alot more flexible
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d_%H:%M:%S");
    std::string timeStr = ss.str(); // This is your readable date/time string

    // Use it in a file name
    std::string filename = "cameraSettings_" + timeStr + ".csv";
    std::ofstream outFile(filename);
    // Print initial camera state
    std::cout << "------ Initial Camera Settings ------\n";
    system("v4l2-ctl --device=/dev/video2 --all");

    // Open camera
    cv::VideoCapture cap(2, cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        std::cerr << "❌ Failed to open camera.\n";
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    cv::namedWindow("Camera Feed", cv::WINDOW_AUTOSIZE);

    // int cv::createTrackbar(const cv::String &trackbarname, const cv::String &winname, int *value, int count, cv::TrackbarCallback onChange = (cv::TrackbarCallback)0, void *userdata = (void *)0)

    // auto_exposure 0x009a0901 (menu)   : min=0 max=3 default=3 value=3 (Aperture       Priority Mode)
    //  1: Manual Mode
    //  3: Aperture Priority Mode
    cv::createTrackbar("Set Auto Exposure (0-1)", "Camera Feed", &setAutoExposureValue, 1, setAutoExposure);

    // exposure_time_absolute 0x009a0902 (int)    : min=3 max=2047 step=1 default=250 value=333 flags=inactive
    cv::createTrackbar("Exposure Time (3-2047)", "Camera Feed", &exposureValue, 2047, setExposureTime);

    // focus_absolute 0x009a090a (int)    : min=0 max=250 step=5 default=0 value=0
    cv::createTrackbar("Set manual focus (0-250)", "Camera Feed", &focusValue, 250, onFocusChange);

    // focus_automatic_continuous 0x009a090c (bool)   : default=1 value=0
    cv::createTrackbar("Auto Focus (0/1)", "Camera Feed", &autofocusEnabled, 1, onAutofocusToggle);

    //  auto_exposure 0x009a0901 (menu)   : min=0 max=3 default=3 value=3 (Aperture Priority Mode
    //  1: Manual Mode
    //  3: Aperture Priority Mode
    cv::createTrackbar("Set Aperature (0/3)", "Camera Feed", &setAperature, 1, onAutofocusToggle);

    // Additional controls for image quality tuning
    cv::createTrackbar("Brightness", "Camera Feed", &brightnessValue, 255, setBrightness);
    cv::createTrackbar("Contrast", "Camera Feed", &contrastValue, 255, setContrast);
    cv::createTrackbar("Saturation", "Camera Feed", &saturationValue, 255, setSaturation);
    cv::createTrackbar("Gain", "Camera Feed", &gainValue, 255, setGain);
    cv::createTrackbar("Sharpness", "Camera Feed", &sharpnessValue, 255, setSharpness);
    cv::createTrackbar("Backlight Comp (0/1)", "Camera Feed", &backlightCompensation, 1, setBacklightComp);
    cv::createTrackbar("Auto WB (0/1)", "Camera Feed", &whiteBalanceAuto, 1, setWhiteBalanceAuto);
    cv::createTrackbar("WB Temp (2000-6500)", "Camera Feed", &whiteBalanceTemperature, 6500, setWhiteBalanceTemperature);

    // Apply initial settings
    setExposureTime(exposureValue, 0);
    onFocusChange(focusValue, 0);
    onAutofocusToggle(autofocusEnabled, 0);
    setAutoExposure(setAperature, 0);
    setBrightness(brightnessValue, 0);
    setContrast(contrastValue, 0);
    setSaturation(saturationValue, 0);
    setGain(gainValue, 0);
    setSharpness(sharpnessValue, 0);
    setBacklightComp(backlightCompensation, 0);
    setWhiteBalanceAuto(whiteBalanceAuto, 0);
    setWhiteBalanceTemperature(whiteBalanceTemperature, 0);

    cv::Mat frame;
    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "⚠️ Blank frame grabbed.\n";
            break;
        }

        cv::imshow("Camera Feed", frame);
        if (cv::waitKey(1) == 'q')
            break;
    }

    cap.release();
    cv::destroyAllWindows();

    // Write CSV header
    outFile << "exposureValue,focusValue,autofocusEnabled,setAperature,brightnessValue,contrastValue,"
            << "saturationValue,gainValue,sharpnessValue,backlightCompensation,whiteBalanceAuto,whiteBalanceTemperature\n";

    // Write CSV data row
    outFile << exposureValue << "," << focusValue << "," << autofocusEnabled << "," << setAperature << ","
            << brightnessValue << "," << contrastValue << "," << saturationValue << "," << gainValue << ","
            << sharpnessValue << "," << backlightCompensation << "," << whiteBalanceAuto << "," << whiteBalanceTemperature << "\n";

    outFile.close();
    return 0;
}
