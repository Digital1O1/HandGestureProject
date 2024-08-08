#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // Open the default camera (usually the first connected camera)
    cv::VideoCapture cap(2);

    // Check if the camera opened successfully
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera" << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    cv::Mat frame, grayscale;
    while (true)
    {
        // Capture frame-by-frame
        cap >> frame;
        cv::cvtColor(frame, grayscale, cv::COLOR_BGR2GRAY);

        // If the frame is empty, break immediately
        if (frame.empty())
        {
            std::cerr << "Error: Frame is empty" << std::endl;
            break;
        }

        // Display the resulting frame
        cv::imshow("USB Camera Feed", frame);
        cv::imshow("Grayscale", grayscale);

        // Press 'q' on the keyboard to exit the loop
        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    // When everything done, release the video capture object
    cap.release();

    // Closes all the frames
    cv::destroyAllWindows();

    return 0;
}
