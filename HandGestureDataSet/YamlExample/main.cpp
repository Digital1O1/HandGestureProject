#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <opencv2/opencv.hpp>

int main()
{
    // === Part 1: Create and Save YAML ===
    YAML::Node config;
    config["gesture"] = "wave";
    config["threshold"] = 0.75;
    config["repetitions"] = 3;

    try
    {
        std::string outputFileName = "OutputYaml.yaml";
        std::ofstream fout(outputFileName);
        fout << config;
        fout.close();
        std::cout << "✅ YAML file saved to : " << outputFileName << "\n\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error writing YAML file: " << e.what() << std::endl;
        return 1;
    }

    // === Part 2: Load and Read YAML ===
    try
    {
        YAML::Node readConfig = YAML::LoadFile("/home/digital101/LinuxCodingFolder/HandGestureProject/HandGestureDataSet/YamlExample/config.yaml");

        int expose = readConfig["exposureValue"].as<int>();

        std::string gesture = readConfig["gesture"].as<std::string>();
        double threshold = readConfig["threshold"].as<double>();
        int repetitions = readConfig["repetitions"].as<int>();

        std::cout << "📖 Loaded YAML content:\n";
        std::cout << "Gesture: " << gesture << std::endl;

        std::cout << "exposureValue: " << expose << std::endl;
        std::cout << "Threshold: " << threshold << std::endl;
        std::cout << "Repetitions: " << repetitions << std::endl;
    }
    catch (const YAML::Exception &e)
    {
        std::cerr << "YAML Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
