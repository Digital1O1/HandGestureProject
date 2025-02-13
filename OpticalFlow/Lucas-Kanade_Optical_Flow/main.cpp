#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <numeric>
#include <cmath>

/*
    Main goals for this program
        - Learn more about the Shi_Tomasi and Harris corner detection algorithms [DONE]
        - Understand why corners are easier to track than edges or flat areas. [DONE]
        - Learn more about how cv::calcOpticalFlowPyrLK() computes the motion of detected points between frames. [DONE]
        - Implement YCrCb color spacing masking
*/

int low_Y = 0, high_Y = 255;
int low_Cr = 0, high_Cr = 255;
int low_Cb = 0, high_Cb = 255;

void displayLightIntensity(double lightingCondition, float qualityLevel)
{
    std::cout << "Light intensity : " << lightingCondition << "\tCurrent qualityLevel set to : " << qualityLevel << std::endl;
}

void on_trackbar(int, void *)
{
    // Get current trackbar values
    cv::namedWindow("Optical Flow - Lucas-Kanade", cv::WINDOW_AUTOSIZE);
    low_Y = cv::getTrackbarPos("Low Y", "Optical Flow - Lucas-Kanade");
    high_Y = cv::getTrackbarPos("High Y", "Optical Flow - Lucas-Kanade");
    low_Cr = cv::getTrackbarPos("Low Cr", "Optical Flow - Lucas-Kanade");
    high_Cr = cv::getTrackbarPos("High Cr", "Optical Flow - Lucas-Kanade");
    low_Cb = cv::getTrackbarPos("Low Cb", "Optical Flow - Lucas-Kanade");
    high_Cb = cv::getTrackbarPos("High Cb", "Optical Flow - Lucas-Kanade");
}

int main()
{
    printf("Listing out available camera devices....\r\n");
    std::system("v4l2-ctl --list-devices");

    bool recalculate = false;
    bool runProgram = true;
    cv::VideoCapture cap(2, cv::CAP_V4L2);

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video capture." << std::endl;
        return -1;
    }

    cv::namedWindow("Optical Flow - Lucas-Kanade");
    cv::createTrackbar("Low Y", "Optical Flow - Lucas-Kanade", &low_Y, 255, on_trackbar);
    cv::createTrackbar("High Y", "Optical Flow - Lucas-Kanade", &high_Y, 255, on_trackbar);
    cv::createTrackbar("Low Cr", "Optical Flow - Lucas-Kanade", &low_Cr, 255, on_trackbar);
    cv::createTrackbar("High Cr", "Optical Flow - Lucas-Kanade", &high_Cr, 255, on_trackbar);
    cv::createTrackbar("Low Cb", "Optical Flow - Lucas-Kanade", &low_Cb, 255, on_trackbar);
    cv::createTrackbar("High Cb", "Optical Flow - Lucas-Kanade", &high_Cb, 255, on_trackbar);

    cv::Mat prevGray, gray, frame;
    cv::Mat ycrcbFrame, frameClone, maskedGray, mask, blendedGray;

    std::vector<cv::Point2f> prevPoints, currPoints;

    // Capture the first frame to initialize previous points
    cap >> frame;
    cv::cvtColor(frame, prevGray, cv::COLOR_BGR2GRAY);

    // Detect initial features to track (e.g., corners or points on hand)
    float qualityLevel = 0.4; // Default value 0.3
    int minDistance = 2;      // Default value 7
    int maxCorners = 100;     // Default value 100 and is the max number of corners/features that can be detected/returned
    bool pointStatus = true;

    // Detect features on the full grayscale image first
    cv::goodFeaturesToTrack(prevGray, prevPoints, maxCorners, qualityLevel, minDistance);

    // Create a background subtractor
    cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::createBackgroundSubtractorMOG2();
    cv::Mat fgMask;

    while (runProgram)
    {
        cap >> frame;
        if (frame.empty())
            break;

        // Convert to grayscale
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Apply background subtraction
        bgSubtractor->apply(frame, fgMask);

        // Cloned so I can pass this frame to a different cvtColor()
        frameClone = frame.clone();
        cv::cvtColor(frameClone, ycrcbFrame, cv::COLOR_BGR2YCrCb);
        cv::inRange(ycrcbFrame, cv::Scalar(low_Y, low_Cr, low_Cb), cv::Scalar(high_Y, high_Cr, high_Cb), mask);

        // Combine both masks (YCrCb mask & background subtraction mask)
        cv::bitwise_and(mask, fgMask, mask);

        // Alternative Fix: Blend original grayscale and masked grayscale
        cv::bitwise_and(gray, gray, maskedGray, mask);
        cv::addWeighted(gray, 0.5, maskedGray, 0.5, 0, blendedGray);

        // Calculate optical flow
        std::vector<uchar> status;
        std::vector<float> err;

        if (!prevPoints.empty())
            cv::calcOpticalFlowPyrLK(prevGray, maskedGray, prevPoints, currPoints, status, err);

        // Print tracked points count
        std::cout << "Tracked Points: " << currPoints.size() << std::endl;

        // Calculate average optical flow magnitude
        double totalFlow = 0;
        int validCount = 0;
        for (size_t i = 0; i < currPoints.size(); i++)
        {
            if (status[i])
            {
                totalFlow += cv::norm(currPoints[i] - prevPoints[i]);
                validCount++;
            }
        }
        if (validCount > 0)
            std::cout << "Average Optical Flow Magnitude: " << (totalFlow / validCount) << std::endl;

        // Calculate lost features
        int lostFeatures = std::count(status.begin(), status.end(), 0);
        std::cout << "Lost Features: " << lostFeatures << " (" << (lostFeatures * 100.0 / status.size()) << "%)" << std::endl;

        // Draw motion vectors
        for (size_t i = 0; i < currPoints.size(); i++)
        {
            if (status[i])
            {
                cv::line(frame, prevPoints[i], currPoints[i], cv::Scalar(0, 255, 0), 2);
                cv::circle(frame, currPoints[i], 5, cv::Scalar(0, 0, 255), -1);
            }
        }

        // Dynamically adjust qualityLevel based on lighting
        double lightingCondition = cv::mean(gray)[0]; // Average grayscale intensity
        if (lightingCondition > 150)
            qualityLevel = 0.5;
        else if (lightingCondition < 50)
            qualityLevel = 0.2;
        else
            qualityLevel = 0.3;

        if (recalculate)
        {
            std::cout << "Recalculating feature points..." << std::endl;
            cv::goodFeaturesToTrack(blendedGray, currPoints, maxCorners, qualityLevel, minDistance);
            recalculate = false;
        }

        prevPoints = currPoints;
        prevGray = gray.clone();

        cv::imshow("Optical Flow - Lucas-Kanade", frame);
        cv::imshow("YCrCb Mask", mask);

        int userInput = cv::waitKey(1);
        if (userInput == 32)
        {
            recalculate = true;
            displayLightIntensity(lightingCondition, qualityLevel);
        }
        else if (userInput == 113)
        {
            std::cout << "EXITING PROGRAM NOW" << std::endl;
            runProgram = false;
        }
        else if (userInput == 45)
        {
            qualityLevel = std::max(0.1f, qualityLevel - 0.1f);
            recalculate = true;
        }
        else if (userInput == 61)
        {
            qualityLevel = std::min(1.0f, qualityLevel + 0.1f);
            recalculate = true;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
