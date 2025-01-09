#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

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

void on_trackbar(int, void *)
{
    // Get current trackbar values
    low_Y = cv::getTrackbarPos("Low Y", "Trackbars");
    high_Y = cv::getTrackbarPos("High Y", "Trackbars");
    low_Cr = cv::getTrackbarPos("Low Cr", "Trackbars");
    high_Cr = cv::getTrackbarPos("High Cr", "Trackbars");
    low_Cb = cv::getTrackbarPos("Low Cb", "Trackbars");
    high_Cb = cv::getTrackbarPos("High Cb", "Trackbars");
}

int main()
{
    bool recalculate = false;
    bool runProgram = true;
    cv::VideoCapture cap(2); // Capture video from the webcam

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video capture." << std::endl;
        return -1;
    }

    cv::namedWindow("Trackbars");
    cv::createTrackbar("Low Y", "Trackbars", &low_Y, 255, on_trackbar);
    cv::createTrackbar("High Y", "Trackbars", &high_Y, 255, on_trackbar);
    cv::createTrackbar("Low Cr", "Trackbars", &low_Cr, 255, on_trackbar);
    cv::createTrackbar("High Cr", "Trackbars", &high_Cr, 255, on_trackbar);
    cv::createTrackbar("Low Cb", "Trackbars", &low_Cb, 255, on_trackbar);
    cv::createTrackbar("High Cb", "Trackbars", &high_Cb, 255, on_trackbar);

    cv::Mat prevGray, gray, frame;
    cv::Mat ycrcbFrame, frameClone, maskedGray, mask;

    std::vector<cv::Point2f> prevPoints, currPoints;

    // Capture the first frame to initialize previous points
    cap >> frame;
    cv::cvtColor(frame, prevGray, cv::COLOR_BGR2GRAY);

    // Detect initial features to track (e.g., corners or points on hand)
    /*
        void cv::goodFeaturesToTrack(InputArray image, OutputArray corners, int maxCorners, double qualityLevel, double minDistance, InputArray mask = <error>, int blockSize = 3, bool useHarrisDetector = false, double k = (0.04))
            - prevGray --> InputArray image
            - prevPoints --> output vector where detected corners are stored
            - 100 --> maxCorners : The max number of features that can be returned by this function
                - Features that exceed this value will be ignored
            - 0.3 --> qualityLevel : A score between 0 and 1
                - Try to stay between 0.1 <= qualityLevel <= 0.5
                    - Both 0 and 1 aren't really that useful
                - Represents minimum quality required for a feature to be considered
                - Features with higher scores --> more likely to be accurate
                - But also increases processing time
            - 7 --> minDistance : minimum possible euclidean distance between detected corners
                - Distance measured in pixels
                - Shi-Tomasi enforces a constraint so no two corners are closer than teh specified minDistance in pixels
                    - If two corners are closer than this distance the function will keep the stronger corner with the higher quality score
                - Establishing a minimum distance will ehlp avoid overcrowding of corners in a small region
                    - Has negative impact on tracking and computational efficiency

        How to systematically determine qualityLevel
            1) Understanding the effect of qualityLevel
                - A higher value close to 1
                    - Retains only the strongest corners
                    - Higher value reduces noise
                    - But could result in missing weaker features like subtle hand movements
                - A lower value close to 0
                    - Increases sensitivity
                    - Captures more corners
                        - But could include noise and less distinct features
            2) Things to understand
                - How cv::goodFeaturesToTrack scores features:
                    - The quality score is based on the smaller eigenvalue of the structure tensor
                        - Eigenvalues represent the amount of variation or intensity changes between two principal directions
                    - High-quality features have high eigenvalues.
                - Lighting and contrast effects:
                    - Poor lighting or low contrast might require a lower qualityLevel to capture sufficient features.
                - Frame rate and motion speed:
                    - Fast movements might require recalculating features more frequently, which affects how sensitive you want the detection.
    */
    float qualityLevel = 0.4; // Default value 0.3
    int minDistance = 2;      // Default value 7
    int maxCorners = 100;     // Default value 100
    bool pointStatus = true;
    // cv::goodFeaturesToTrack(prevGray, prevPoints, maxCorners, qualityLevel, minDistance);
    cv::goodFeaturesToTrack(maskedGray, prevPoints, maxCorners, qualityLevel, minDistance);

    // YCrCb values
    cv::Scalar ycrcbLower(0, 133, 77);
    cv::Scalar ycrcbUpper(255, 173, 127);
    while (runProgram)
    {
        cap >> frame;
        if (frame.empty())
            break;

        // Convert to grayscale

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Cloned so I can pass this frame to a different cvtColor()
        frameClone = frame.clone();
        cv::cvtColor(frameClone, ycrcbFrame, cv::COLOR_BGR2YCrCb);
        cv::inRange(ycrcbFrame, cv::Scalar(low_Y, low_Cr, low_Cb), cv::Scalar(high_Y, high_Cr, high_Cb), mask);

        //     cv::bitwise_and(image1, image2, result);
        cv::bitwise_and(gray, gray, maskedGray, mask);

        cv::Mat prevPtsMat;
        cv::Mat(prevPoints).convertTo(prevPtsMat, CV_32F);

        // Check if prevPtsMat is empty
        if (prevPtsMat.empty())
        {
            std::cout << "Previous points are empty." << std::endl;
            pointStatus = false;
            // break;
        }
        else
            pointStatus = true;

        // Calculate optical flow
        std::vector<uchar> status;
        std::vector<float> err;

        /*
            void cv::calcOpticalFlowPyrLK(InputArray prevImg, InputArray nextImg, InputArray prevPts, InputOutputArray nextPts, OutputArray status, OutputArray err, cv::Size winSize = cv::Size(21, 21), int maxLevel = 3, cv::TermCriteria criteria = cv::TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 30, (0.01)), int flags = 0, double minEigThreshold = (1.0E-4))
                - prevGray → InputArray prevImg : Previous frame in grayscale.
                - gray → InputArray nextImg : Current frame in grayscale.
                - prevPoints → InputArray prevPts: Points in prevGray to track.
                - currPoints → InputOutputArray nextPts: Output points in gray (tracked positions).
                - status → OutputArray status: Indicates if tracking was successful for each point.
                - err → OutputArray err: Errors associated with each tracked point.
        */
        if (pointStatus)
            cv::calcOpticalFlowPyrLK(prevGray, maskedGray, prevPoints, currPoints, status, err);

        // cv::calcOpticalFlowPyrLK(prevGray, gray, prevPoints, currPoints,
        //                          status, err);

        // Filter points based on status and ensure they're valid
        std::vector<cv::Point2f> validPoints;

        // Draw the motion vectors
        for (size_t i = 0; i < currPoints.size(); i++)
        {
            if (status[i])
            {
                // void cv::line(InputOutputArray img, cv::Point pt1, cv::Point pt2, const cv::Scalar &color, int thickness = 1, int lineType = 8, int shift = 0)
                cv::line(frame, prevPoints[i], currPoints[i],
                         cv::Scalar(0, 255, 0), 2);
                cv::circle(frame, currPoints[i], 5, cv::Scalar(0, 0, 255),
                           -1);
                validPoints.push_back(currPoints[i]);
            }
        }
        // cv::mean() returns a scalar object
        cv::Scalar avgIntensity = cv::mean(gray);
        double lightingCondition = avgIntensity[0]; // Average grayscale intensity
        std::cout << "Light intensity : " << lightingCondition << "\tCurrent qualtiyLevel set to : " << qualityLevel << std::endl;

        // Dynamically adjust qualityLevel based on lighting
        if (lightingCondition > 150)
        { // Bright
            qualityLevel = 0.5;
        }
        else if (lightingCondition < 50)
        { // Dim
            qualityLevel = 0.2;
        }
        else
        { // Moderate lighting
            qualityLevel = 0.3;
        }

        // Check if valid points are less than 5 and recalculate if necessary
        if ((std::count(status.begin(), status.end(), 1) < 5) || recalculate)
        {
            std::cout << "Points recalculated..." << std::endl;
            // std::cout << "Current qualityLevel set to :  " << qualityLevel << std::endl;

            // void cv::goodFeaturesToTrack(InputArray image, OutputArray corners, int maxCorners, double qualityLevel, double minDistance, InputArray mask = noArray(), int blockSize = 3, bool useHarrisDetector = false, double k = (0.04))

            cv::goodFeaturesToTrack(maskedGray, currPoints, maxCorners, qualityLevel, minDistance);

            // cv::goodFeaturesToTrack(gray, currPoints, maxCorners, qualityLevel, minDistance);
            validPoints = currPoints; // Use the newly detected points
            recalculate = false;
        }

        // Update prevPoints with valid points
        prevPoints = validPoints;

        // Draw the motion vectors for valid points
        for (size_t i = 0; i < prevPoints.size(); i++)
        {
            cv::circle(frame, prevPoints[i], 5, cv::Scalar(0, 0, 255), -1);
        }

        // Display result
        cv::imshow("Optical Flow - Lucas-Kanade", frame);
        cv::imshow("Masked Grayscale", maskedGray);
        cv::imshow("YCrCb Mask", mask);

        // Update for next frame
        prevGray = gray.clone();
        prevPoints = currPoints;

        int userInput = cv::waitKey(1);
        // std::cout << "User key : " << userInput << std::endl;

        switch (userInput)
        {
        case 32: // Press spacebar to recalculate points to be detected
        {
            // std::cout << "SPACEBAR" << std::endl;
            recalculate = true;
            break;
        }
        case 113: // Exit the program
        {
            std::cout << "EXITING PROGRAM NOW" << std::endl;
            runProgram = false;
            break;
        }

        case 45: // '-' key to decrease qualityLevel
        {
            qualityLevel = std::max(0.1f, qualityLevel - 0.1f);
            std::cout << "Decreased qualityLevel: " << qualityLevel << std::endl;
            recalculate = true;
            break;
        }

        case 61: // '=' key to increase qualityLevel
        {
            qualityLevel = std::min(1.0f, qualityLevel + 0.1f);
            std::cout << "Increased qualityLevel: " << qualityLevel << std::endl;
            recalculate = true;
            break;
        }
        } // End of switch statement

        // if (cv::waitKey(1) == 'q')
        //     break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
