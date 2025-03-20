#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#define MAXTHRESH 255
// These values work great if lamp is on
int treshVal = 98; // With logitec camera
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
    cv::createTrackbar("treshVal", "Convex Hull Detection", &treshVal, MAXTHRESH);
    cv::createTrackbar("depthLevel", "Convex Hull Detection", &depthLevel, MAXTHRESH);

    // Open camera (device index 2, using V4L2 API)
    cv::VideoCapture cap(2, cv::CAP_V4L2);

    double width = 640;
    double height = 480;

    // System calls that OpenCV can't control

    // Disable Auto White Balance
    int ret1 = system("v4l2-ctl --set-ctrl=white_balance_automatic=1");

    if (ret1 == 0)
        std::cout << "Auto Exposure set to 1 (Manual Mode).\n";
    else
        std::cerr << "Failed to disable auto exposure.\n";

    system("v4l2-ctl --set-ctrl=brightness=10");
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    std::cin.get();
    // cap.set(cv::CAP_PROP_AUTO_WB, 0); // Must use v4l2-ctl --set-ctrl=auto_exposure=1 to disable auto white balance
    //  cap.set(cv::CAP_PROP_AUTO_WB, 0);
    //   cap.set(cv::CAP_PROP_BRIGHTNESS, 96);
    //   cap.set(cv::CAP_PROP_CONTRAST, 96);
    //   cap.set(cv::CAP_PROP_SATURATION, 96);

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
        cv::threshold(blurred, thresh, treshVal, MAXTHRESH, cv::THRESH_BINARY); // If pixel greater than thresVal, set it to 255, other than that set it to 0

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
                    float depth = defects[i][3] / 256.0;                          // Defect depth (conexity defect depth) that measures how deep the indentation between two points on the conex hull on a person's hand contour

                    // Camera parameters
                    double autoWB = cap.get(cv::CAP_PROP_AUTO_WB);
                    double wbTemp = cap.get(cv::CAP_PROP_WB_TEMPERATURE);

                    std::cout << "Defect " << i << ": " << std::endl;
                    std::cout << "  Start: (" << start.x << ", " << start.y << ")" << std::endl;
                    std::cout << "  End: (" << end.x << ", " << end.y << ")" << std::endl;
                    std::cout << "  Far: (" << far.x << ", " << far.y << ")" << std::endl;
                    std::cout << "  Depth: " << depth << std::endl;
                    std::cout << "Auto White Balance: " << autoWB << std::endl;
                    std::cout << "White Balance Temperature: " << wbTemp << std::endl;

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
