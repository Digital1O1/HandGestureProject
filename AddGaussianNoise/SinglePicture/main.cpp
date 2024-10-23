#include <opencv2/opencv.hpp>
#include <random>

int main()
{

    // Load the image
    cv::Mat image = cv::imread("/home/digital101/LinuxCodingFolder/HandGestureProject/AddGaussianNoise/Images/1603970279-pexels-photo-3-636x325.jpg");

    // Create a random number generator
    /*
        default_random_engine
            - Random number engine class that's responsible for generateing the RAW RANDOM NUMBERS
                - It pretty much initializes the random number generator
            - Those numbers are used by normal_distribution to produce numbers that follow the Gaussian distribution
            - Every time distribution(generator) is called
                - The generator provides a random value
                - The distribution transofrms it to fit the desired normal distribution
    */
    std::default_random_engine generator;

    /*
        The mean in normal_distribution
            - Is the 'center' or the average value around which the random numbers are distributed
            - When you add Gaussian noise to an image will determine the 'baseline' value of the noise
            - Changing the mean will determine where the 'center' fo the noise distribution lies
                - Increasing the mean : shifting the entire distribution of noise values towards positive values
                    - Mean = 0: Noise will cause fluctuations that average out to zero over the entire image, leading to a balanced "grainy" effect with darkening and brightening happening roughly equally.
                    - Mean > 0: Noise will cause the image to brighten overall, since more noise values will be positive, pulling the pixel values upward.
                    - Mean < 0: Noise will darken the image, as most noise values will be negative, pulling pixel values downward.
                - Example : Increasing mean to 10
                    - Gaussian noise will mostly generate values cenetered around 10
                    - With some values higher/lower depending on the SD

    */
    std::normal_distribution<double> distribution(0, 100); // Mean 0, standard deviation 10
    cv::Mat edgeDetectionWithNoise, edgeDetectionNoNoise;
    // Add Gaussian noise to the image
    cv::Mat noisyImage = image.clone();

    /*
        void cv::Canny(InputArray image, OutputArray edges, double threshold1, double threshold2, int apertureSize = 3, bool L2gradient = false)
    
        +1 overload

        Finds edges in an image using the Canny algorithm

        Parameters:
            image – 8-bit input image.
            edges – output edge map; single channels 8-bit image, which has the same size as image .
            threshold1 – first threshold for the hysteresis procedure.
            threshold2 – second threshold for the hysteresis procedure.
            apertureSize – aperture size for the Sobel operator.
            L2gradient – a flag, indicating whether a more accurate \f$L_2norm =\sqrt{(dI/dx)^2 + (dI/dy)^2}should be used to calculate the image gradient magnitude ( L2gradient=true ), or whether the default \f$L_1norm =|dI/dx|+|dI/dy|is enough ( L2gradient=false ).
    
    */
    cv::Canny(image, edgeDetectionNoNoise, 100, 150);
    cv::Canny(noisyImage, edgeDetectionWithNoise, 100, 150);

    // Get a pointer to the pixel data (unsigned char* for 8-bit image)
    uchar *imageDataPtr = noisyImage.data;

    // Loop over all pixels (Note: image is row-major ordered)
    int numRows = noisyImage.rows;
    int numColsWithColorChannels = noisyImage.cols * noisyImage.channels(); // Multiply cols by number of channels to get number of elements

    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numColsWithColorChannels; ++col)
        {

            double noise = distribution(generator);

            // Access the pixel value at (row, col) based on the pointer offset.
            // row * numColsWithColorChannels -> gives you the starting index of the row in the flat array
            // + col -> gives the specific position in that row

            /*
                Example : 2x2 image with 3 channels RGB can be represented as

                Row 0: [R, G, B, R, G, B]
                Row 1: [R, G, B, R, G, B]

                Image has
                    - rows = 2
                    - cols = 2
                    - channels = 3
                    - So numColsWithColorChannels = cols * channels = 2 * 3 = 6
                    - imageDataPtr would point to a continuous block of 12 bytes
                        - 6 bytes per row * 2 rows

                row * numColsWithColorChannels gives STARTING INDEX of that row
                Adding col within that row lets you access the correct channel for each pixel
                    - Row 0, Column 0, Channel R: Index = 0 * 6 + 0 = 0
                    - Row 0, Column 0, Channel G: Index = 0 * 6 + 1 = 1
                    - Row 0, Column 0, Channel B: Index = 0 * 6 + 2 = 2
                    - Row 0, Column 1, Channel R: Index = 0 * 6 + 3 = 3


            */
            int pixelValue = imageDataPtr[row * numColsWithColorChannels + col];

            // Add Gaussian noise to the pixel value
            pixelValue += static_cast<int>(noise);

            // Clamp the pixel value to the range [0, 255] to prevent overflow
            imageDataPtr[row * numColsWithColorChannels + col] = cv::saturate_cast<uchar>(pixelValue);
        }
    }

    // Display the original and noisy images
    cv::imshow("Original Image", image);
    cv::imshow("Noisy Image", noisyImage);
    cv::imshow("edgeDetectionNoNoise", edgeDetectionNoNoise);
    cv::imshow("edgeDetectionWithNoise", edgeDetectionWithNoise);

    cv::waitKey(0);

    return 0;
}