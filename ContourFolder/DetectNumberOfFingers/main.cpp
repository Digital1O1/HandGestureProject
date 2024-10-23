#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

// Function to detect and draw hand contours and convexity defects
void detectHandContours(Mat &frame)
{
    Mat gray, blurred, thresholdOutput;

    // Convert the image to grayscale
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // Apply Gaussian Blur to reduce noise
    GaussianBlur(gray, blurred, Size(5, 5), 0);

    // Threshold the image to binary (black and white)
    threshold(blurred, thresholdOutput, 60, 255, THRESH_BINARY_INV);

    // Find contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresholdOutput, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Check if any contour was detected
    if (!contours.empty())
    {
        // Find the largest contour (which is most likely the hand)
        int largestContourIndex = 0;
        double maxArea = contourArea(contours[0]);
        for (int i = 1; i < contours.size(); i++)
        {
            double area = contourArea(contours[i]);
            if (area > maxArea)
            {
                maxArea = area;
                largestContourIndex = i;
            }
        }

        // Draw the largest contour
        drawContours(frame, contours, largestContourIndex, Scalar(0, 255, 0), 2);

        // Find the convex hull and draw it
        vector<int> hullIndices;
        convexHull(contours[largestContourIndex], hullIndices, true);

        vector<Point> hullPoints;
        for (int i : hullIndices)
        {
            hullPoints.push_back(contours[largestContourIndex][i]);
        }
        polylines(frame, hullPoints, true, Scalar(0, 0, 255), 2);

        // Detect convexity defects
        vector<Vec4i> defects;
        if (hullIndices.size() > 3)
        { // Minimum size for convexity defects calculation
            convexityDefects(contours[largestContourIndex], hullIndices, defects);

            int fingerCount = 0;
            for (const Vec4i &defect : defects)
            {
                Point start = contours[largestContourIndex][defect[0]];
                Point end = contours[largestContourIndex][defect[1]];
                Point far = contours[largestContourIndex][defect[2]];
                float depth = defect[3] / 256.0; // Convert from fixed-point

                // Filter defects by depth (only consider deep defects between fingers)
                if (depth > 10)
                {
                    circle(frame, far, 5, Scalar(255, 0, 0), -1);  // Mark defect points
                    line(frame, start, far, Scalar(255, 0, 0), 2); // Line from start to defect point
                    line(frame, far, end, Scalar(255, 0, 0), 2);   // Line from defect point to end

                    fingerCount++; // Counting fingers based on convexity defects
                }
            }

            // Display finger count
            putText(frame, "Fingers: " + to_string(fingerCount), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        }
    }
}

int main()
{
    // Open the default camera
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cerr << "Error opening video capture." << endl;
        return -1;
    }

    Mat frame;
    while (cap.read(frame))
    {
        // Detect and draw hand contours
        detectHandContours(frame);

        // Show the result
        imshow("Hand Gesture Detection", frame);

        // Exit on pressing the ESC key
        if (waitKey(30) == 27)
            break;
    }

    return 0;
}
