#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <chrono>
#include <ctime>
#include <sstream>
#define MAXTHRESH 255
// Globals for trackbars
int exposureValue = 3;
int focusValue = 30;
int autofocusEnabled = 1;
int setAperature = 3;
int setAutoExposureValue = 3;
int setPanAbsoluteValue = 0;

int treshVal = 75; // With logitec camera
int depthLevel = 10;

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
    // qcv::namedWindow("Convex Hull Settings");

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

    // Threshold
    cv::createTrackbar("Threshold", "Camera Settings", &treshVal, 255);
    cv::createTrackbar("Depth", "Camera Settings", &depthLevel, 20); // Adjust max as needed

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

    cv::Mat frame, gray, blurred, thresh;
    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "⚠️ Blank frame grabbed.\n";
            break;
        }

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
        cv::threshold(blurred, thresh, treshVal, MAXTHRESH, cv::THRESH_BINARY); // If pixel greater than

        // Find contours
        std::vector<std::vector<cv::Point>> contours;

        /*
            Notes about void cv::findContours(InputArray image, OutputArrayOfArrays contours, int mode, int method, cv::Point offset = cv::Point())

            Second parameter controls how contours are retrieved (mode)
            cv::RETR_EXTERNAL	Retrieves only the outermost contours, ignoring any nested contours inside.
            cv::RETR_LIST	    Retrieves all contours but doesn’t maintain hierarchical relationships (i.e., doesn't differentiate between outer and inner contours).
            cv::RETR_CCOMP	    Retrieves all contours and organizes them into two levels (outer contours as level 1, inner holes as level 2).
            cv::RETR_TREE	    Retrieves all contours and stores the full hierarchy (useful for nested contours like rings).

            Third parameter controls how the contours are approximated (method)
            cv::CHAIN_APPROX_NONE	Stores all contour points. Results in very detailed and large contours.
            cv::CHAIN_APPROX_SIMPLE	Removes redundant points and only stores corner points, significantly reducing memory usage.
            cv::CHAIN_APPROX_TC89_L1	Applies the Teh-Chin approximation to compress the contour further.
            cv::CHAIN_APPROX_TC89_KCOS	Similar to TC89_L1, but uses a different heuristic for contour approximation.
        */
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

        if (largestContourIdx != -1)
        {
            /*
                What is a convex hull
                    - Set of points in n-dimensional space that's the smallest convex polygon that contains all the points
                        - Convex polygon : A polygon where all the verticies point OUTWARD
                            - Meaning that if you drew a line connecting two points on the polygon, the line would always lie entirely within or on the boundary of the polygon
                            - In OpenCV a convex polygon is defined as a polygon that has all it's verticies lying on one side of the polygon
                    - It's also a fundamental concept in computational geometery and has numerous applications in CS, graphics and other fields

                More learning resources about convex hulls ---> https://learnopencv.com/convex-hull-using-opencv-in-python-and-c/
            */
            // Compute convex hull (for drawing)
            std::vector<cv::Point> hull;
            cv::convexHull(contours[largestContourIdx], hull);
            int numHullPoints = hull.size(); // Store convex hull point count

            // Compute convex hull (for finding defects)
            std::vector<int> hullIndices;
            cv::convexHull(contours[largestContourIdx], hullIndices, false, false);
            std::sort(hullIndices.begin(), hullIndices.end());

            // Compute convexity defects
            int numDefects = 0;
            std::vector<cv::Vec4i> defects;

            if (hullIndices.size() > 3) // Need at least 4 points to compute defects
            {
                cv::convexityDefects(contours[largestContourIdx], hullIndices, defects);
                numDefects = defects.size(); // Store defect count

                for (size_t i = 0; i < defects.size(); i++)
                {
                    cv::Point start = contours[largestContourIdx][defects[i][0]]; // Start of defect
                    cv::Point end = contours[largestContourIdx][defects[i][1]];   // End of defect
                    cv::Point far = contours[largestContourIdx][defects[i][2]];   // Deepest point of defect
                    float depth = defects[i][3] / 256.0;                          // Defect depth (conexity defect depth) that measures how deep the
                                                                                  // indentation between two points on the
                                                                                  // conex hull on a person's hand contour

                    // Camera parameters
                    // double autoWB = cap.get(cv::CAP_PROP_AUTO_WB);
                    // double wbTemp = cap.get(cv::CAP_PROP_WB_TEMPERATURE);

                    // std::cout << "Defect " << i << ": " << std::endl;
                    // std::cout << "  Start: (" << start.x << ", " << start.y << ")" << std::endl;
                    // std::cout << "  End: (" << end.x << ", " << end.y << ")" << std::endl;
                    // std::cout << "  Far: (" << far.x << ", " << far.y << ")" << std::endl;
                    // std::cout << "  Depth: " << depth << std::endl;
                    // std::cout << "Auto White Balance: " << autoWB << std::endl;
                    // std::cout << "White Balance Temperature: " << wbTemp << std::endl;

                    // The greater 'depth' is signifies there's a considerable amount of space between fingers or the fact that the fingers are  seperated
                    // depth measured in terms of pixels
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

            // std::cout << "Convex Hull Points: " << numHullPoints << std::endl;

            // Bounding box
            cv::Rect bbox = cv::boundingRect(contours[largestContourIdx]);
            // std::cout << "Bounding Box - Width: " << bbox.width << ", Height: " << bbox.height << std::endl;

            // Aspect ratio
            double aspect_ratio = (double)bbox.width / bbox.height;
            // std::cout << "Aspect Ratio: " << aspect_ratio << std::endl;

            // Area and Perimeter
            double area = cv::contourArea(contours[largestContourIdx]);
            double perimeter = cv::arcLength(contours[largestContourIdx], true);
            // std::cout << "Contour Area: " << area << ", Perimeter: " << perimeter << std::endl;

            // Draw contours and convex hull
            cv::drawContours(frame, contours, largestContourIdx, cv::Scalar(0, 255, 0), 2);
            cv::polylines(frame, hull, true, cv::Scalar(255, 0, 0), 2);
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
    cameraSettings["threshVal"] = treshVal;
    cameraSettings["depthLevel"] = depthLevel;

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
