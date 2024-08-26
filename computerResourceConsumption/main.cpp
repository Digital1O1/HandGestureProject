#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <cstdio>

// Function to set terminal to non-blocking mode
void setNonBlockingMode()
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

// Function to reset terminal to default settings
void resetTerminalMode()
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO); // Enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void printCPUUsage(std::ofstream &file)
{
    std::string command = "top -bn1 | grep \"Cpu(s)\" | "
                          "sed \"s/.*, *\\([0-9.]*\\)%* id.*/\\1/\" | "
                          "awk '{print 100 - $1}'";
    char buffer[128];
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
        std::string cpuUsage = result.substr(0, result.size() - 1) + "%";
        file << cpuUsage << ",";
        std::cout << "Overall CPU Usage: " << cpuUsage << std::endl;
    }
}

void printCPUUsagePerCore(std::ofstream &file)
{
    std::string command = "mpstat -P ALL 1 1 | awk '/Average/ && $2 ~ /[0-9]/ {print 100 - $12}'";
    char buffer[128];
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
        std::istringstream iss(result);
        std::string line;
        int core = 0;
        while (std::getline(iss, line))
        {
            std::string coreUsage = line + "%";
            file << coreUsage << ",";
            std::cout << "Core " << core << " CPU Usage: " << coreUsage << std::endl;
            ++core;
        }
    }
}

void printMemoryUsage(std::ofstream &file)
{
    std::string command = "free -m | awk 'NR==2{printf \"%s,%s,%.2f\\n\", $3,$2,$3*100/$2 }'";
    char buffer[128];
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
        std::istringstream iss(result);
        std::string usedMemory, totalMemory, memoryUsage;
        std::getline(iss, usedMemory, ',');
        std::getline(iss, totalMemory, ',');
        std::getline(iss, memoryUsage, '\n');
        file << usedMemory << "," << totalMemory << "," << memoryUsage << ",";
        std::cout << "Memory Usage: " << usedMemory << "MB/" << totalMemory << "MB (" << memoryUsage << "%)" << std::endl;
    }
}

void printTemperature(std::ofstream &file)
{
    std::ifstream tempFile("/sys/class/thermal/thermal_zone0/temp");
    if (tempFile.is_open())
    {
        double temperature;
        tempFile >> temperature;
        tempFile.close();
        std::string tempStr = std::to_string(temperature / 1000.0) + "°C";
        file << tempStr << std::endl;
        std::cout << "Temperature: " << tempStr << std::endl;
    }
    else
    {
        std::cerr << "Error: Unable to read temperature!" << std::endl;
        file << "N/A" << std::endl;
    }
}

int main()
{
    setNonBlockingMode(); // Set terminal to non-blocking mode

    std::string filename = "output.csv";
    std::ofstream file(filename);

    // Write CSV header
    file << "Overall CPU Usage,Core 0 CPU Usage,Core 1 CPU Usage,Core 2 CPU Usage,Core 3 CPU Usage,Core 4 CPU Usage,Core 5 CPU Usage,Core 6 CPU Usage,Core 7 CPU Usage,Core 8 CPU Usage,Core 9 CPU Usage,Core 10 CPU Usage,Core 11 CPU Usage,Used Memory (MB),Total Memory (MB),Memory Usage (%),Temperature (°C)\n";

    std::cout << "Monitoring system metrics. Press 'q' to stop." << std::endl;

    char c;
    while (true)
    {
        c = getchar();

        if (c != EOF && c == 'q')
        {
            std::cout << "Exiting program now." << std::endl;

            break;
        }

        printCPUUsage(file);
        printCPUUsagePerCore(file);
        printMemoryUsage(file);
        printTemperature(file);

        std::cout << "======================================" << std::endl;

        // std::this_thread::sleep_for(std::chrono::seconds(1)); // Adjust the sleep time as needed
    }

    resetTerminalMode(); // Reset terminal to default settings
    std::cout << "Logged system metrics to " << filename << std::endl;

    return 0;
}
