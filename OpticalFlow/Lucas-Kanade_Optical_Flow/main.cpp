#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

/*
    Main goals for this program
        - Learn moreabout the Shi_Tomasi and Harris corner detection aglorithms
        - Understand why corners are easier to track than edges or flat areas.
        - Learn more about how cv::calcOpticalFlowPyrLK() computes the motion of detected points between frames.
        - Implement YCrCb color spacing masking
*/
int main()
{
    bool recalculate = false;
    bool runProgram = true;
    cv::VideoCapture cap(0); // Capture video from the webcam

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video capture." << std::endl;
        return -1;
    }

    cv::Mat prevGray, gray, frame;
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
                - Represents minimum quality required for a feature to be considered
                - Features with higher scores --> more likely to be accurate
                - But also increases processing time
            - 7 --> minDistance : minimum distance between features in pixels
    */
    float qualityLevel = 0.4; // Default value 0.3
    int minDistance = 2;      // Default value 7
    int maxCorners = 100;     // Default value 100
    cv::goodFeaturesToTrack(prevGray, prevPoints, maxCorners, qualityLevel, minDistance);

    while (runProgram)
    {
        cap >> frame;
        if (frame.empty())
            break;

        // Convert to grayscale
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

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
        
        cv::calcOpticalFlowPyrLK(prevGray, gray, prevPoints, currPoints,
                                 status, err);

        // Filter points based on status and ensure they're valid
        std::vector<cv::Point2f> validPoints;

        // Draw the motion vectors
        for (size_t i = 0; i < currPoints.size(); i++)
        {
            if (status[i])
            {
                cv::line(frame, prevPoints[i], currPoints[i],
                         cv::Scalar(0, 255, 0), 2);
                cv::circle(frame, currPoints[i], 5, cv::Scalar(0, 0, 255),
                           -1);
                validPoints.push_back(currPoints[i]);
            }
        }

        // Check if valid points are less than 5 and recalculate if necessary
        if ((std::count(status.begin(), status.end(), 1) < 5) || recalculate)
        {
            std::cout << "Points recalculated..." << std::endl;
            cv::goodFeaturesToTrack(gray, currPoints, maxCorners, qualityLevel, minDistance);
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

        // Update for next frame
        prevGray = gray.clone();
        prevPoints = currPoints;

        int userInput = cv::waitKey(1);
        // std::cout << "User key : " << userInput << std::endl;

        switch (userInput)
        {
        case 32:
        {
            // std::cout << "SPACEBAR" << std::endl;
            recalculate = true;
            break;
        }
        case 113:
        {
            std::cout << "EXITING PROGRAM NOW" << std::endl;
            runProgram = false;
            break;
        }
        }

        // if (cv::waitKey(1) == 'q')
        //     break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}