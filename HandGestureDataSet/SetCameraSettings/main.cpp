#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <chrono>
#include <ctime>
#include <sstream>

// Globals for trackbars
int exposureValue = 3;
int focusValue = 30;
int autofocusEnabled = 1;
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
        std::cout << "✅ Command executed.\n";
    else if (result == 127)
        std::cout << "❌ 'v4l2-ctl' not found.\n";
    else
    {
        if (result == 65280)
        {
            std::cout << "⚠️ TURN OFF ANY 'AUTO' SETTINGS PRIOR TO ADJUSTING CAMERA VALUES " << "\n";
        }
        std::cout << "⚠️ Command failed. Code: " << result << "\n";
    }
}

// Callback functions
void setAutoExposure(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=auto_exposure=" + std::to_string(value == 0 ? 1 : 3));
}
void setExposureTime(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=exposure_time_absolute=" + std::to_string(value));
}
void onFocusChange(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_absolute=" + std::to_string(value));
}
void onAutofocusToggle(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_automatic_continuous=" + std::to_string(value));
}
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

int main()
{
    std::cout << "------ Initial Camera Settings ------\n";
    system("v4l2-ctl --device=/dev/video2 --all");

    cv::VideoCapture cap(2, cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        std::cerr << "❌ Failed to open camera.\n";
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    cv::namedWindow("Camera Feed");
    cv::namedWindow("Camera Settings");

    cv::createTrackbar("Set Auto Exposure (0-1)", "Camera Settings", &setAutoExposureValue, 1, setAutoExposure);
    cv::createTrackbar("Auto Focus (0/1)", "Camera Settings", &autofocusEnabled, 1, onAutofocusToggle);
    cv::createTrackbar("Auto WB (0/1)", "Camera Settings", &whiteBalanceAuto, 1, setWhiteBalanceAuto);
    cv::createTrackbar("Exposure Time (3-2047)", "Camera Settings", &exposureValue, 2047, setExposureTime);
    cv::createTrackbar("Set manual focus (0-250)", "Camera Settings", &focusValue, 250, onFocusChange);
    cv::createTrackbar("WB Temp (2000-6500)", "Camera Settings", &whiteBalanceTemperature, 6500, setWhiteBalanceTemperature);
    cv::createTrackbar("Set Aperature (0/3)", "Camera Settings", &setAperature, 1, onAutofocusToggle);
    cv::createTrackbar("Brightness", "Camera Settings", &brightnessValue, 255, setBrightness);
    cv::createTrackbar("Contrast", "Camera Settings", &contrastValue, 255, setContrast);
    cv::createTrackbar("Saturation", "Camera Settings", &saturationValue, 255, setSaturation);
    cv::createTrackbar("Gain", "Camera Settings", &gainValue, 255, setGain);
    cv::createTrackbar("Sharpness", "Camera Settings", &sharpnessValue, 255, setSharpness);
    cv::createTrackbar("Backlight Comp (0/1)", "Camera Settings", &backlightCompensation, 1, setBacklightComp);

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

    cv::moveWindow("Camera Feed", 100, 100);
    cv::moveWindow("Camera Settings", 800, 100);

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

        int key = cv::waitKey(1);
        if (key == 'q' || key == 113) // Either use the character 'q' or the ASCII numerical value for 'q'
            break;
    }

    cap.release();
    cv::destroyAllWindows();

    // Save all settings to YAML
    YAML::Node cameraSettings;
    cameraSettings["exposureValue"] = exposureValue;
    cameraSettings["focusValue"] = focusValue;
    cameraSettings["autofocusEnabled"] = autofocusEnabled;
    cameraSettings["setAperature"] = setAperature;
    cameraSettings["setAutoExposureValue"] = setAutoExposureValue;
    cameraSettings["brightness"] = brightnessValue;
    cameraSettings["contrast"] = contrastValue;
    cameraSettings["saturation"] = saturationValue;
    cameraSettings["gain"] = gainValue;
    cameraSettings["sharpness"] = sharpnessValue;
    cameraSettings["backlightCompensation"] = backlightCompensation;
    cameraSettings["whiteBalanceTemperature"] = whiteBalanceTemperature;
    cameraSettings["whiteBalanceAuto"] = whiteBalanceAuto;

    try
    {
        // Timestamping stuff

        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        // stringstream since it accepts complex values and foramts them into strings and is alot more flexible
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d_%H:%M:%S");
        std::string timeStr = ss.str(); // This is your readable date/time string
        std::string filename = "Camera_Settings_" + timeStr + ".yaml";
        std::ofstream cameraOutputFile(filename);
        cameraOutputFile << cameraSettings;
        cameraOutputFile.close();
        std::cout << "✅ Camera settings saved to Camera_Settings.yaml\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error writing YAML file: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
