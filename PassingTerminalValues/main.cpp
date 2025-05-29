#include <iostream>
#include <fstream>
#include <string>


// argc --> argument count : an integer representing the number of arguments passed that includes
//          the program itself

// argv --> argument vector : An array of C-style strings (char*) that contains the actual arguments 
//          argv[0] is the program itself
//          argv[1] is the first argument 
int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        std::cout << "No arguments passed exiting program now...." << std::endl;
        return 1;
    }

    std::cout << "Variables recieved from terminal" << std::endl;

    for (int i = 1; i < argc; ++i)
    {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }

    std::string gesture_label = argv[1];
    std::string csvLabel = argv[2];
    csvLabel += ".csv";

    std::ofstream file(csvLabel, std::ios::app);
    if (file.is_open())
    {
        // Replace these with your actual values
        int numHullPoints = 10;
        int numDefects = 3;
        int width = 100;
        int height = 150;
        float aspect_ratio = static_cast<float>(width) / height;
        float area = 2000.0f;
        float perimeter = 300.0f;

        file << numHullPoints << ","
             << numDefects << ","
             << width << ","
             << height << ","
             << aspect_ratio << ","
             << area << ","
             << perimeter << ","
             << gesture_label << "\n";
    }

    return 0;
}