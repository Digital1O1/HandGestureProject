#include <iostream>
#include <filesystem>
#include <unordered_set> // has table-based container that stores unique elements only

namespace fs = std::filesystem;

int main()
{
    std::string fileExtensionTarget = ".csv";

    // std::unordered_set<TypeOfTheKey>NameAssigned
    std::unordered_set<std::string> file_names;

    /*
        Nitty gritty stuff going on in the for loop
            - & is used to create a reference to each directory entry object rather than copying it
            - This is done due to the metadata and path information
                - Copying all that is wasteful
        Why the const keyword is used
            - Since we're not modifying any entry object, it's used
            - Gives clear intent to the reader
    */
    for (const auto &entry : fs::directory_iterator(fs::current_path()))
    {
        if (entry.is_regular_file() && entry.path().extension() == fileExtensionTarget)
        {
            std::string name = entry.path().filename().string();
            // std::cout << "Checking duplicate file : " << name << std::endl;
            std::cout << entry.path() << " " << entry.filename().string() << std::endl;
            if (file_names.find(name) != file_names.end())
            {
                std::cerr << "⚠️ Duplicate CSV file detected: " << name << std::endl;
                std::cerr << "Please rename or remove the duplicate file." << std::endl;
                return 1; // exit with error
            }

            file_names.insert(name);
        }
    }

    std::cout << "✅ No duplicate CSV file names found." << std::endl;
    return 0;
}
