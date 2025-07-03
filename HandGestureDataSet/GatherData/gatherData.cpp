#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <unordered_set>

namespace fs = std::filesystem;

#define MAXTHRESH 255

int dynamicThresh = 0;
// ----------------- Convex Hull Variables ----------------- //
// These values work with Camera_Settings_2025-06-11_16:52:41.yaml
int treshVal = 75; // With logitec camera
int depthLevel = 10;

// ----------------- TrackBar Variables ----------------- //
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

int main(int argc, char *argv[])
{
    // Expected parameters to be passed
    // 1) Gesture label

    if (argc < 3)
    {
        printf("\n\nEnter name of GESTURE to be recorded and if you want Dynamic thresholding applied (true/false)\r\n\n");

        exit(1);
    }

    // Values passed via CLI
    std::string gesture_label = argv[1];
    std::string csvLabel = argv[1];
    std::string dynamicArg = argv[2];
    bool dynamicThresholdFlag = (dynamicArg == "true");
    std::cout << argv[2] << "\t" << dynamicThresholdFlag;

    csvLabel += ".csv";

    // Serach for existing CSV files
    std::string target_extension = ".csv";
    std::unordered_set<std::string> file_names;

    for (const auto &entry : fs::directory_iterator(fs::current_path()))
    {
        if (entry.is_regular_file() && entry.path().extension() == target_extension)
        {
            std::string name = entry.path().filename().string();

            if (file_names.find(name) != file_names.end())
            {
                std::cerr << "⚠️ Duplicate CSV file detected: " << name << std::endl;
                std::cerr << "Please rename or remove the duplicate file." << std::endl;
                return 1; // exit with error
            }

            file_names.insert(name);
        }
    }

    std::cout << "✅ No duplicate CSV file names found." << std::endl;
    // --------------- Time stuff for CSV --------------//
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // stringstream since it accepts complex values and foramts them into strings and is alot more flexible
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d_%H:%M:%S");
    std::string timeStr = ss.str(); // This is your readable date/time string

    // -------------- CSV STUFF -------------- //
    std::ofstream file(csvLabel, std::ios::app); // Append mode

    // std::ofstream file("hand_gesture_data.csv", std::ios::app); // Append mode

    // Check if the file is empty before writing the headers
    if (file.tellp() == 0) // If file is empty, write header
    {
        // Write this to the CSV header
        file << "threshVal,depthLevel,numHullPoints,numDefects,bbox.width,bbox.height,aspect_ratio,area,perimeter,gesture_label\n";
    }

    // -------------- Read from YAML -------------- //
    try
    {
        YAML::Node readConfig = YAML::LoadFile("/home/digital101/LinuxCodingFolder/HandGestureProject/HandGestureDataSet/GatherData/Camera_Settings_2025-06-28_15:41:29.yaml");

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

        // Set the values
        setBrightness(brightnessValue, nullptr);
        setContrast(contrastValue, nullptr);
        setSaturation(saturationValue, nullptr);
        setGain(gainValue, nullptr);
        setSharpness(sharpnessValue, nullptr);
        setBacklightComp(backlightCompensation, nullptr);
        setWhiteBalanceTemperature(whiteBalanceTemperature, nullptr);
        setWhiteBalanceAuto(whiteBalanceAuto, nullptr);

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
    }
    catch (const YAML::Exception &e)
    {
        std::cerr << "YAML Error: " << e.what() << std::endl;
        return 1;
    }

    // std::cout << "Press ENTER to continue...." << std::endl;
    // std::cin.get();

    // -------------- Camera -------------- //
    cv::VideoCapture cap(2, cv::CAP_V4L2);

    double width = 640;
    double height = 480;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    // cv::namedWindow("Camera Feed", cv::WINDOW_AUTOSIZE);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return 0;
    }

    // Trackbars
    cv::namedWindow("Convex Hull Detection", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("treshVal", "Convex Hull Detection", &treshVal, MAXTHRESH);
    cv::createTrackbar("depthLevel", "Convex Hull Detection", &depthLevel, MAXTHRESH);
    cv::Mat frame, gray, blurred, thresh;

    // auto now = std::chrono::steady_clock::now();
    // int secondsUntilNextSample = timeOut - std::chrono::duration_cast<std::chrono::seconds>(now - lastSampleTime).count();

    cv::waitKey(1000);
    int recordDuration = 5; // in seconds
    auto startTime = std::chrono::steady_clock::now();

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        if (dynamicThresholdFlag)
        {
            cv::Scalar meanIntensity = cv::mean(gray);
            dynamicThresh = static_cast<int>(meanIntensity[0] * 0.85); // 'Taking' whatever percentage
                                                                       //  Of the average brightness of the
                                                                       //  Grayscale to use as s threshold
                                                                       //  Value
                                                                       // Adjust this number to keep more (increase it) of the background
                                                                       // Example : 0.9 --> Keep more of background but could be too conservative
                                                                       //           0.7 --> More aggressive but might  lose finger details in shadow
                                                                       //           8.5 --> Good balance
            dynamicThresh = std::clamp(dynamicThresh, 30, 150);

            treshVal = dynamicThresh; // Update global or trackbar value
        }

        // Convert to grayscale and apply threshold
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
        cv::threshold(blurred, thresh, treshVal, MAXTHRESH, cv::THRESH_BINARY); // If pixel greater than thresVal, set it to 255, other than that set it to 0

        // Calculate brightness based on grayscale image
        // Assume you already have gray (grayscale image)

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

            std::cout << "Convex Hull Points: " << numHullPoints << std::endl;

            // Bounding box
            cv::Rect bbox = cv::boundingRect(contours[largestContourIdx]);
            std::cout << "Bounding Box - Width: " << bbox.width << ", Height: " << bbox.height << std::endl;

            // Aspect ratio
            double aspect_ratio = (double)bbox.width / bbox.height;
            std::cout << "Aspect Ratio: " << aspect_ratio << std::endl;

            // Area and Perimeter
            double area = cv::contourArea(contours[largestContourIdx]);
            double perimeter = cv::arcLength(contours[largestContourIdx], true);
            std::cout << "Contour Area: " << area << ", Perimeter: " << perimeter << std::endl;
            std::cout << "dynamicThresh : " << dynamicThresh << std::endl;

            // Draw contours and convex hull
            cv::drawContours(frame, contours, largestContourIdx, cv::Scalar(0, 255, 0), 2);
            cv::polylines(frame, hull, true, cv::Scalar(255, 0, 0), 2);

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
            int remainingTime = recordDuration - static_cast<int>(elapsed);
            if (remainingTime <= 0)
                break;

            else
            { // --------------- Write values to CSV file ---------------
                if (file.is_open())
                {
                    file << treshVal << ","
                         << depthLevel << ","
                         << numHullPoints << ","
                         << numDefects << ","
                         << bbox.width << ","
                         << bbox.height << ","
                         << aspect_ratio << ","
                         << area << ","
                         << perimeter << ","
                         << csvLabel << "\n";
                }
            }

            // Draw countdown on frame
            std::string countdownText = "Recording ends in: " + std::to_string(remainingTime) + "s";
            cv::putText(frame, countdownText, cv::Point(50, 50),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
        }

        // Display result
        cv::imshow("Convex Hull Detection", frame);
        if (cv::waitKey(1) == 'q')
            break;
    }

    // Close the file properly after the loop
    file.close();

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
