#include <opencv2/opencv.hpp>
#include <iostream>

int blurAmount = 1;
int maxBlurAmount = 30;
cv::Mat image, blurredImage;

void onTrackbar(int value, void *userdata)
{
    blurAmount = value;
    std::string stringValue = "Blur amount : " + std::to_string(blurAmount);
    std::string baseString = "Blur amount :";
    std::cout << stringValue << std::endl;
    // std::cout << "Blur value : " << blurAmount << std::endl;
    cv::GaussianBlur(image, blurredImage, cv::Size(blurAmount * 2 + 1, blurAmount * 2 + 1), 0);
    cv::imshow("Blurred Image", blurredImage);
}

int main()
{
    image = cv::imread("/home/digital101/LinuxCodingFolder/HandGestureProject/AdjustCameraSettingTrackbars/ph_024_043_pw1.jpg");
    if (image.empty())
    {
        std::cout << "Could not open or find the image!\n"
                  << std::endl;
        return -1;
    }
    // cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
    // cv::imshow("Original Image", image);

    cv::namedWindow("Blurred Image", cv::WINDOW_AUTOSIZE);

    cv::createTrackbar("Blur Amount", "Blurred Image", &blurAmount, maxBlurAmount, onTrackbar);
    onTrackbar(blurAmount, 0); // Initialize display

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}