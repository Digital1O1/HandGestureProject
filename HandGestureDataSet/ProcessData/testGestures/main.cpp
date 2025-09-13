#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <algorithm> // CHANGE: for std::max_element

#define MAXTHRESH 255

// ----------------- Globals ----------------- //
int threshVal = 1; // With logitec camera
int depthLevel = 1;

// Camera control globals (from YAML)
int exposureValue = 3;
int focusValue = 30;
int autofocusEnabled = 1;
int setAperature = 3;
int setAutoExposureValue = 3;
int setPanAbsoluteValue = 0;
int brightnessValue = 128;
int contrastValue = 128;
int saturationValue = 128;
int gainValue = 255;
int sharpnessValue = 128;
int backlightCompensation = 0;
int whiteBalanceTemperature = 4000;
int whiteBalanceAuto = 1;
int onAutofocusToggleValue = 0;

// ----------------- Helper: Run v4l2-ctl ----------------- //
void runCommand(const std::string &command)
{
    int result = system(command.c_str());
    std::cout << "\n[CMD] " << command << std::endl;
    if (result == 0)
        std::cout << "✔️ Command executed successfully\n";
    else if (result == 127)
        std::cout << "❌ 'v4l2-ctl' not found in PATH.\n";
    else
        std::cout << "⚠️ Error executing command. Code: " << result << "\n";
}

// ----------------- Camera Control Callbacks ----------------- //
void setAutoExposure(int value, void *)
{
    // auto_exposure 0x009a0901 (menu)   : min=0 max=3 default=3 value=3 (Aperture Priority Mode)
    //  1: Manual Mode
    //  3: Aperture Priority Mode
    if (value == 0)
        runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=auto_exposure=1");
    else
        runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=auto_exposure=3");
}

void onAutofocusToggle(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_automatic_continuous=" + std::to_string(value));
}

void setExposureTime(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=exposure_time_absolute=" + std::to_string(value));
}

void onFocusChange(int value, void *)
{
    runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=focus_absolute=" + std::to_string(value));
}

void setBrightness(int value, void *) { runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=brightness=" + std::to_string(value)); }
void setContrast(int value, void *) { runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=contrast=" + std::to_string(value)); }
void setSaturation(int value, void *) { runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=saturation=" + std::to_string(value)); }
void setGain(int value, void *) { runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=gain=" + std::to_string(value)); }
void setSharpness(int value, void *) { runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=sharpness=" + std::to_string(value)); }
void setBacklightComp(int value, void *) { runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=backlight_compensation=" + std::to_string(value)); }
void setWhiteBalanceTemperature(int value, void *) { runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=white_balance_temperature=" + std::to_string(value)); }
void setWhiteBalanceAuto(int value, void *) { runCommand("v4l2-ctl --device=/dev/video2 --set-ctrl=white_balance_automatic=" + std::to_string(value)); }

// ----------------- Main ----------------- //
// int main()
int main(int argc, char *argv[])
{

    if (argc < 1)
    {
        std::cout << "No arguments passed exiting program now...." << std::endl;
        return 1;
    }

    std::cout << "Variables recieved from terminal" << std::endl;

    for (int i = 1; i < argc; ++i)
    {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }

    std::string YamlPath = argv[1];

    std::cout << YamlPath << std::endl;
    std::cin.get();

    // ---------- Load YAML Camera Settings ---------- //
    try
    {
        YAML::Node readConfig = YAML::LoadFile("/home/digital101/LinuxCodingFolder/HandGestureProject/HandGestureDataSet/SetCameraSettings/Camera_Settings_2025-09-04_18:30:45.yaml");

        exposureValue = readConfig["exposureValue"].as<int>();
        focusValue = readConfig["focusValue"].as<int>();
        autofocusEnabled = readConfig["autofocusEnabled"].as<int>();
        setAperature = readConfig["setAperature"].as<int>();
        setAutoExposureValue = readConfig["setAutoExposureValue"].as<int>();
        brightnessValue = readConfig["brightness"].as<int>();
        contrastValue = readConfig["contrast"].as<int>();
        saturationValue = readConfig["saturation"].as<int>();
        gainValue = readConfig["gain"].as<int>();
        sharpnessValue = readConfig["sharpness"].as<int>();
        backlightCompensation = readConfig["backlightCompensation"].as<int>();
        whiteBalanceTemperature = readConfig["whiteBalanceTemperature"].as<int>();
        whiteBalanceAuto = readConfig["whiteBalanceAuto"].as<int>();
        threshVal = readConfig["threshVal"].as<int>();
        depthLevel = readConfig["depthLevel"].as<int>();
        // onAutofocusToggleValue = readConfig["onAutofocusToggleValue"].as<int>();

        // Set the values
        setBrightness(brightnessValue, nullptr);
        setContrast(contrastValue, nullptr);
        setSaturation(saturationValue, nullptr);
        setGain(gainValue, nullptr);
        setSharpness(sharpnessValue, nullptr);
        setBacklightComp(backlightCompensation, nullptr);
        setWhiteBalanceTemperature(whiteBalanceTemperature, nullptr);
        setWhiteBalanceAuto(whiteBalanceAuto, nullptr);
        setAutoExposure(setAutoExposureValue, nullptr);
        onAutofocusToggle(onAutofocusToggleValue, nullptr);
        setExposureTime(exposureValue, nullptr);

        std::cout << "\n\n✅ Loaded camera settings from YAML:\n";
        std::cout << "Exposure: " << exposureValue << "\n";
        std::cout << "Focus: " << focusValue << "\n";
        std::cout << "Autofocus Enabled: " << autofocusEnabled << "\n";
        std::cout << "Aperture: " << setAperature << "\n";
        std::cout << "Auto Exposure: " << setAutoExposureValue << "\n";
        std::cout << "Brightness: " << brightnessValue << "\n";
        std::cout << "Contrast: " << contrastValue << "\n";
        std::cout << "Saturation: " << saturationValue << "\n";
        std::cout << "Gain: " << gainValue << "\n";
        std::cout << "Sharpness: " << sharpnessValue << "\n";
        std::cout << "Backlight Compensation: " << backlightCompensation << "\n";
        std::cout << "White Balance Temperature: " << whiteBalanceTemperature << "\n";
        std::cout << "White Balance Auto: " << whiteBalanceAuto << "\n";
        std::cout << "Threshold value : " << threshVal << "\n";
        std::cout << "DepthLevel value : " << depthLevel << "\n";
    }
    catch (const YAML::Exception &e)
    {
        std::cerr << "YAML Error: " << e.what() << std::endl;
        return 1;
    }

    // ---------- ONNX Runtime Setup ---------- //
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "gesture");
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);
    Ort::Session session(env,
                         "/home/digital101/LinuxCodingFolder/HandGestureProject/HandGestureDataSet/ProcessData/gesture_svm.onnx",
                         session_options);
    Ort::AllocatorWithDefaultOptions allocator;

    auto input_name = session.GetInputNameAllocated(0, allocator);
    auto output_name = session.GetOutputNameAllocated(0, allocator);

    // ---------- Camera Setup ---------- //
    cv::VideoCapture cap(2, cv::CAP_V4L2);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    if (!cap.isOpened())
    {
        std::cerr << "❌ Cannot open camera\n";
        return -1;
    }

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        // ----------------- Step 1: Preprocessing -----------------
        // if (dynamicThresholdFlag)
        // {
        //     cv::Scalar meanIntensity = cv::mean(gray);
        //     dynamicThresh = static_cast<int>(meanIntensity[0] * 0.85);
        //     dynamicThresh = std::clamp(dynamicThresh, 30, 150);
        //     threshVal = dynamicThresh; // Update global threshold value
        // }

        // Convert to grayscale and apply threshold
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
        cv::threshold(blurred, thresh, threshVal, MAXTHRESH, cv::THRESH_BINARY);

        // ----------------- Step 2: Find Contours -----------------
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Find the largest contour (likely the hand)
        int largestContourIdx = -1;
        double maxArea = 0;
        for (size_t i = 0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            if (area > maxArea)
            {
                maxArea = area;
                largestContourIdx = i;
            }
        }

        // CHANGE: correctly handle "no contour" case and continue
        if (largestContourIdx == -1)
        {
            std::cout << "No contour found in this frame." << std::endl;
            cv::imshow("Gesture Detection", frame);
            if (cv::waitKey(1) == 'q')
                break;
            continue;
        }

        // ----------------- Step 3: Convex Hull -----------------
        std::vector<cv::Point> hull;
        cv::convexHull(contours[largestContourIdx], hull);
        int numHullPoints = (int)hull.size();

        // Compute convex hull indices for defects
        std::vector<int> hullIndices;
        cv::convexHull(contours[largestContourIdx], hullIndices, false, false);
        std::sort(hullIndices.begin(), hullIndices.end());

        int numDefects = 0;
        std::vector<cv::Vec4i> defects;

        // ----------------- Step 4: Convexity Defects -----------------
        if (hullIndices.size() > 3) // need at least 4
        {
            try
            {
                cv::convexityDefects(contours[largestContourIdx], hullIndices, defects);
                numDefects = (int)defects.size();

                for (size_t i = 0; i < defects.size(); i++)
                {
                    cv::Point start = contours[largestContourIdx][defects[i][0]];
                    cv::Point end = contours[largestContourIdx][defects[i][1]];
                    cv::Point far = contours[largestContourIdx][defects[i][2]];
                    float depth = defects[i][3] / 256.0f;

                    if (depth > depthLevel)
                    {
                        cv::circle(frame, far, 5, cv::Scalar(0, 0, 255), -1);
                        cv::circle(frame, start, 5, cv::Scalar(255, 0, 0), -1);
                        cv::circle(frame, end, 5, cv::Scalar(255, 0, 0), -1);
                        cv::line(frame, start, far, cv::Scalar(0, 255, 255), 2);
                        cv::line(frame, end, far, cv::Scalar(0, 255, 255), 2);
                    }
                }
            }
            catch (const cv::Exception &e)
            {
                std::cerr << "⚠️ ConvexityDefects error: " << e.what() << std::endl;
                numDefects = 0; // fail safe
            }
        }

        // ----------------- Step 5: Features -----------------
        cv::Rect bbox = cv::boundingRect(contours[largestContourIdx]);
        double aspect_ratio = (double)bbox.width / bbox.height;
        double area = cv::contourArea(contours[largestContourIdx]);
        double perimeter = cv::arcLength(contours[largestContourIdx], true);

        // Prepare features for SVM/ONNX
        std::vector<float> features = {
            (float)threshVal,
            (float)depthLevel,
            (float)numHullPoints,
            (float)numDefects,
            (float)bbox.width,
            (float)bbox.height,
            (float)aspect_ratio,
            (float)area,
            (float)perimeter};

        // ----------------- Step 6: Run Inference -----------------
        auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        std::array<int64_t, 2> input_shape{1, (int64_t)features.size()};
        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
            memory_info, features.data(), features.size(), input_shape.data(), input_shape.size());

        // Input and output names (as const char* arrays, not std::string)
        const char *input_names[] = {input_name.get()};
        const char *output_names[] = {output_name.get()};

        // Run inference
        auto output_tensors = session.Run(Ort::RunOptions{nullptr},
                                          input_names, &input_tensor, 1,
                                          output_names, 1);

        // CHANGE: Robust output handling (scalar OR vector)
        Ort::Value &out = output_tensors.front();
        auto info = out.GetTensorTypeAndShapeInfo();
        auto out_shape = info.GetShape();
        size_t elem_count = info.GetElementCount();
        float *prediction = out.GetTensorMutableData<float>();

        int predicted_class = 0;
        if (elem_count == 1)
        {
            // SVM exported as single label
            predicted_class = static_cast<int>(std::lround(prediction[0]));
        }
        else
        {
            // Vector of scores/probabilities → argmax
            size_t argmax = std::distance(prediction,
                                          std::max_element(prediction, prediction + elem_count));
            predicted_class = static_cast<int>(argmax);

            // Optional: dump first few values for verification
            // std::cout << "Output shape: ";
            // for (auto d : out_shape) std::cout << d << " ";
            // std::cout << " | ";
            // for (size_t i = 0; i < elem_count; ++i)
            //     std::cout << prediction[i] << (i+1<elem_count?", ":"");
            // std::cout << std::endl;
        }

        std::cout << "Predicted Gesture: " << predicted_class << std::endl;
        for (float f : features)
            std::cout << f << " ";
        std::cout << std::endl;
        std::string gestureString = "Gesture " + std::to_string(predicted_class);
        cv::putText(frame, gestureString, cv::Point(25, 25),
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);

        // ----------------- Step 7: Visualization -----------------
        cv::drawContours(frame, contours, largestContourIdx, cv::Scalar(0, 255, 0), 2);
        cv::polylines(frame, hull, true, cv::Scalar(255, 0, 0), 2);

        // CHANGE: overlay live debug to confirm YAML-applied values & features
        std::ostringstream dbg;
        dbg << "threshold=" << threshVal << " depth=" << depthLevel
            << " hull=" << numHullPoints << " defects=" << numDefects;
        cv::putText(frame, dbg.str(), cv::Point(25, 55),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);

        // ----------------- Step 8: Show frame -----------------
        cv::imshow("Gesture Detection", frame);
        if (cv::waitKey(1) == 'q')
            break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
