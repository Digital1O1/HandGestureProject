#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

#define MAXTHRESH 255
// These values work great if lamp is on
int treshVal = 130;
int depthLevel = 6;

int main()
{
    // --------------- CSV STUFF --------------- //
    std::ofstream file("hand_gesture_data.csv", std::ios::app); // Append mode

    // Check if the file is empty before writing the headers
    if (file.tellp() == 0) // If file is empty, write header
    {
        file << "num_convex_hull_points,num_convexity_defects,bounding_box_width,bounding_box_height,aspect_ratio,contour_area,contour_perimeter,label\n";
    }

    // Create trackbars for threshold and depth level
    cv::namedWindow("Convex Hull Detection", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("treshVal", "Convex Hull Detection", &treshVal, 255);
    cv::createTrackbar("depthLevel", "Convex Hull Detection", &depthLevel, 255);

    // Open camera (device index 2, using V4L2 API)
    cv::VideoCapture cap(2, cv::CAP_V4L2);

    double width = 640;
    double height = 480;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return 0;
    }

    cv::Mat frame, gray, blurred, thresh;

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        // Convert to grayscale and apply threshold
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
        cv::threshold(blurred, thresh, treshVal, MAXTHRESH, cv::THRESH_BINARY);

        // Find contours
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

        if (largestContourIdx != -1)
        {
            // Compute convex hull (for drawing)
            std::vector<cv::Point> hull;
            cv::convexHull(contours[largestContourIdx], hull);
            int numHullPoints = hull.size(); // Store convex hull point count

            std::cout << "Convex Hull Points: " << numHullPoints << std::endl;

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
                    float depth = defects[i][3] / 256.0;                          // Defect depth

                    std::cout << "Defect " << i << ": " << std::endl;
                    std::cout << "  Start: (" << start.x << ", " << start.y << ")" << std::endl;
                    std::cout << "  End: (" << end.x << ", " << end.y << ")" << std::endl;
                    std::cout << "  Far: (" << far.x << ", " << far.y << ")" << std::endl;
                    std::cout << "  Depth: " << depth << std::endl;

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

            // Draw contours and convex hull
            cv::drawContours(frame, contours, largestContourIdx, cv::Scalar(0, 255, 0), 2);
            cv::polylines(frame, hull, true, cv::Scalar(255, 0, 0), 2);

            // --------------- Write values to CSV file ---------------
            if (file.is_open())
            {
                file << numHullPoints << ","
                     << numDefects << ","
                     << bbox.width << ","
                     << bbox.height << ","
                     << aspect_ratio << ","
                     << area << ","
                     << perimeter << ","
                     << "gesture_name" << "\n"; // Replace "gesture_name" with actual label so you don't hate life later when it comes to manually labeling everything
            }
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
