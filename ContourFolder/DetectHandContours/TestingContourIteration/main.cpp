#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

int main()
{
    // Load or create a binary image
    cv::Mat image = cv::imread("path_to_your_image", cv::IMREAD_GRAYSCALE);
    cv::Mat binaryImage;
    cv::threshold(image, binaryImage, 127, 255, cv::THRESH_BINARY);

    // Parameters
    int maxIterations = 10;
    double stabilizationThreshold = 0.05; // Stop when area change is below 5%
    double previousArea = 0;
    std::vector<double> areaChanges;

    // Structuring element for erosion (3x3 rectangle)
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    for (int i = 1; i <= maxIterations; ++i)
    {
        // Apply erosion
        cv::Mat erodedImage;
        cv::erode(binaryImage, erodedImage, kernel, cv::Point(-1, -1), i);

        // Find contours
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(erodedImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Calculate total contour area
        double totalArea = 0;
        for (const auto &contour : contours)
        {
            totalArea += cv::contourArea(contour);
        }
        areaChanges.push_back(totalArea);

        // Calculate percentage change in area
        if (i > 1)
        {
            double areaChange = std::abs(totalArea - previousArea) / previousArea;
            std::cout << "Iteration " << i << ", Total Area: " << totalArea
                      << ", Change: " << areaChange * 100 << "%" << std::endl;

            // Stop if area change falls below threshold
            if (areaChange < stabilizationThreshold)
            {
                std::cout << "Area stabilized at iteration " << i << std::endl;
                break;
            }
        }

        previousArea = totalArea;
    }

    return 0;
}
