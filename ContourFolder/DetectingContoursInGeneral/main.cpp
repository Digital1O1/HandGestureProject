#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main()
{
    // Open the default camera
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video stream!" << std::endl;
        return -1;
    }

    // Window for displaying video
    cv::namedWindow("Contours Detection", cv::WINDOW_AUTOSIZE);

    while (true)
    {
        cv::Mat frame;
        cap >> frame; // Capture each frame
        if (frame.empty())
        {
            std::cerr << "Error: Could not capture frame!" << std::endl;
            break;
        }

        // Convert to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Apply Gaussian blur to reduce noise
        cv::Mat blurred;
        // void cv::GaussianBlur(InputArray src, OutputArray dst, cv::Size ksize, double sigmaX, double sigmaY = (0.0), int borderType = 4)
        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

        // Use Canny edge detector to find edges
        cv::Mat edges;
        // void cv::Canny(InputArray image, OutputArray edges, double threshold1, double threshold2, int apertureSize = 3, bool L2gradient = false)
        cv::Canny(blurred, edges, 100, 200);

        // Find contours from the edges
        std::vector<std::vector<cv::Point>> contours;

        // Vec4i --> vector with 4 dimensions
        std::vector<cv::Vec4i> hierarchy;

        cv::findContours(edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        /*
            Notes about findContours()

            Syntax : void cv::findContours(InputArray image, OutputArrayOfArrays contours, OutputArray hierarchy, int mode, int method, cv::Point offset = cv::Point())

            int mode
                - See retrevial modes : https://docs.opencv.org/4.10.0/d3/dc0/group__imgproc__shape.html#ga819779b9857cc2f8601e6526a3a5bc71
                - It's a contour retrieval mode in OpenCV that controls how contours are retrieved and organized
                - Specifically it builds a hierarchical representation of the contours in the image
                - What does cv::RETR_TREE actually do?
                    - Retrieves all contours in the image
                    - Reconstructs a full hierarchy of nested contours
                        - Example : There's a contour inside another contour, kinda like a hole inside an object.
                        - cv::RETR_TREE will represent this relationship as a tree structure
                            - Outer contours
                                - Objects
                                - Are considered parent contours
                            - Inner contours
                                - Holes or nested shapes
                                - Considered children of the outer contours
                    - Hierarchy array
                        - Hierarchy info is stored in hierarchy parameter
                        - This array contains info about the relationship between contours like
                            - The index of the next contour at the same hierarchical level.
                            - The index of the previous contour at the same hierarchical level.
                            - The index of the first child (inner contour).
                            - The index of the parent (outer contour).
                - Practical Example of cv::RETR_TREE:
                    - Imagine detecting a circular ring (like a donut).
                    - The outer contour of the ring is the parent,
                        -The hole in the middle of the ring is the child contour.
                        -cv::RETR_TREE will represent this parent-child relationship,
                            - Allowing you to understand the structural layout of nested contours.

                - When to Use cv::RETR_TREE:
                    -Use cv::RETR_TREE when you need detailed information about the relationships between contours,
                        - Such as parent-child (nested) contours.
                    - It is especially useful when dealing with images that have multiple nested objects or holes
                        - (e.g., when identifying objects inside other objects).

                If you don’t need this hierarchical information, you could use simpler modes like cv::RETR_EXTERNAL (which only retrieves outermost contours).

            int method() | enum cv::ContourApproximationModes
                - CHAIN_APPROX_NONE
                    - Python: cv.CHAIN_APPROX_NONE
                    - Stores absolutely all the contour points.
                    - That is, any 2 subsequent points (x1,y1) and (x2,y2) of the contour will be either horizontal, vertical or diagonal neighbors, that is, max(abs(x1-x2),abs(y2-y1))==1.

                CHAIN_APPROX_SIMPLE | THIS IS WHAT'S CURRENTLY BEING USED
                    - Python: cv.CHAIN_APPROX_SIMPLE
                    - compresses horizontal, vertical, and diagonal segments and leaves only their end points. For example, an up-right rectangular contour is encoded with 4 points.

                CHAIN_APPROX_TC89_L1
                    - Python: cv.CHAIN_APPROX_TC89_L1
                    - applies one of the flavors of the Teh-Chin chain approximation algorithm [265]

                CHAIN_APPROX_TC89_KCOS
                    - Python: cv.CHAIN_APPROX_TC89_KCOS
                    - applies one of the flavors of the Teh-Chin chain approximation algorithm [265]


                - enum RetrievalModes {}
                - Depending on the enum value picked will dictate the countours chosen
                    - Values range from 0 -> 4

        */

        // Draw the contours on the original frame
        cv::Mat contourOutput = frame.clone();

        // void cv::drawContours(InputOutputArray image, InputArrayOfArrays contours, int contourIdx, const cv::Scalar &color, int thickness = 1, int lineType = 8, InputArray hierarchy = <error>, int maxLevel = <error>, cv::Point offset = cv::Point())

        cv::drawContours(contourOutput, contours, -1, cv::Scalar(0, 255, 0), 2); // Green contours

        // Show the original frame with contours
        cv::imshow("Contours Detection", contourOutput);

        // Break the loop if 'q' is pressed
        char key = (char)cv::waitKey(1);
        if (key == 'q')
        {
            break;
        }
    }

    // Release the camera and close windows
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
