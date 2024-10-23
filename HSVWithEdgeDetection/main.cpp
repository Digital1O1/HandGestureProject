#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/bgsegm.hpp>

using namespace cv;
using namespace std;

#define USBCAMERA 2

// Global variables for trackbar positions
int lowerH = 0, upperH = 20;
int lowerS = 48, upperS = 255;
int lowerV = 50, upperV = 255;
int cannyLowerThreshold = 50, cannyUpperThreshold = 150;

void createTrackbars()
{
    namedWindow("Trackbars", WINDOW_AUTOSIZE);

    // Create HSV trackbars
    createTrackbar("Lower H (HSV)", "Trackbars", &lowerH, 179);
    createTrackbar("Upper H (HSV)", "Trackbars", &upperH, 179);
    createTrackbar("Lower S (HSV)", "Trackbars", &lowerS, 255);
    createTrackbar("Upper S (HSV)", "Trackbars", &upperS, 255);
    createTrackbar("Lower V (HSV)", "Trackbars", &lowerV, 255);
    createTrackbar("Upper V (HSV)", "Trackbars", &upperV, 255);

    // Create Canny edge detection trackbars
    createTrackbar("Lower Canny Threshold", "Trackbars", &cannyLowerThreshold, 255);
    createTrackbar("Upper Canny Threshold", "Trackbars", &cannyUpperThreshold, 255);
}

// Function to display histograms for the HSV channels
Mat createHistogram(const Mat &image, int width)
{
    vector<Mat> hsv_planes;
    split(image, hsv_planes);

    int histSize = 256;
    float range[] = {0, 256};
    const float *histRange = {range};
    bool uniform = true, accumulate = false;
    Mat h_hist, s_hist, v_hist;

    calcHist(&hsv_planes[0], 1, 0, Mat(), h_hist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&hsv_planes[1], 1, 0, Mat(), s_hist, 1, &histSize, &histRange, uniform, accumulate);
    calcHist(&hsv_planes[2], 1, 0, Mat(), v_hist, 1, &histSize, &histRange, uniform, accumulate);

    int hist_w = width, hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

    normalize(h_hist, h_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(s_hist, s_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(v_hist, v_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    for (int i = 1; i < histSize; i++)
    {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(h_hist.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(h_hist.at<float>(i))),
             Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(s_hist.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(s_hist.at<float>(i))),
             Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(v_hist.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(v_hist.at<float>(i))),
             Scalar(0, 0, 255), 2, 8, 0);
    }

    // Add a legend to the histogram
    putText(histImage, "Hue", Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 0, 0), 2);
    putText(histImage, "Saturation", Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
    putText(histImage, "Value", Point(10, 90), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2);

    return histImage;
}

int main()
{
    VideoCapture usbCamera(USBCAMERA);

    if (!usbCamera.isOpened())
    {
        cerr << "Can't open camera\n\n";
        return -1;
    }

    usbCamera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    usbCamera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    Mat frame, hsvImage, mask, combinedImage, gray_frame, blurred_frame, edges;

    // Create the trackbars
    createTrackbars();

    // Create a window for the histogram
    namedWindow("HSV Histogram", WINDOW_AUTOSIZE);

    while (true)
    {
        usbCamera >> frame;

        if (frame.empty())
        {
            cerr << "Error: Frame is empty" << endl;
            break;
        }

        // Convert to HSV color space
        cvtColor(frame, hsvImage, COLOR_BGR2HSV);

        // Apply skin color segmentation in HSV
        Scalar lower_hsv(lowerH, lowerS, lowerV);
        Scalar upper_hsv(upperH, upperS, upperV);
        inRange(hsvImage, lower_hsv, upper_hsv, mask);

        // Bitwise-AND the mask and the original frame to get the skin regions
        Mat skin;
        bitwise_and(frame, frame, skin, mask);

        // Convert the skin region image to grayscale
        cvtColor(skin, gray_frame, COLOR_BGR2GRAY);

        // Apply Gaussian blur to reduce noise
        GaussianBlur(gray_frame, blurred_frame, Size(5, 5), 1.5);

        // Apply Canny edge detector using the trackbar values
        Canny(blurred_frame, edges, cannyLowerThreshold, cannyUpperThreshold);

        // Convert the single-channel edge-detected image to a 3-channel image for concatenation
        Mat edges_3channel, finalImage;
        cvtColor(edges, edges_3channel, COLOR_GRAY2BGR);

        // Concatenate the original frame, the mask, and the edge-detected image horizontally
        hconcat(skin, edges_3channel, combinedImage);
        hconcat(combinedImage, frame, finalImage);
        // Display the results
        imshow("Skin Detection | Canny Edges", finalImage);

        // Display the histogram in a separate window
        Mat histImage = createHistogram(hsvImage, 640);
        imshow("HSV Histogram", histImage);

        if (waitKey(1) == 'q')
        {
            break;
        }
    }

    return 0;
}
