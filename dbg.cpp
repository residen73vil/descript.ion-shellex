#include "dbg.h"
//objects for debug build
#ifdef _DEBUG
#include <windows.h>
#include <regex>
std::wofstream dbgout;


bool fileExists(const std::string& filename) {
    FILE* file = fopen(filename.c_str(), "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

std::string getBaseName(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    size_t lastDot = path.find_last_of('.');
    
    if (lastDot == std::string::npos || (lastSlash != std::string::npos && lastDot < lastSlash)) {
        return path.substr(lastSlash + 1); // No extension
    }
    return path.substr(lastSlash + 1, lastDot - lastSlash - 1);
}

std::string getExtension(const std::string& path) {
    size_t lastDot = path.find_last_of('.');
    if (lastDot == std::string::npos) {
        return ""; // No extension
    }
    return path.substr(lastDot);
}

std::string getDirectory(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos) {
        return ""; // No directory
    }
    return path.substr(0, lastSlash + 1);
}

void dbg_open_log_file(const char* log_file) {
    std::string originalPath = log_file;
    
    // Check if the original file exists
    if (!fileExists(originalPath)) {
        // Open the file for writing
		dbgout.open(originalPath);
        return;
    }

    std::string baseName = getBaseName(originalPath); // Get the file name without extension
    std::string extension = getExtension(originalPath); // Get the file extension

    // Regex to find a number at the end of the base name
    std::regex numberRegex("(\\d+)$");
    std::smatch match;

    int newNumber = 1; // Start numbering from 1
    std::string newBaseName = baseName;

    // Check if the base name ends with a number
    if (std::regex_search(baseName, match, numberRegex)) {
        // Extract the number and increment it
        newNumber = std::stoi(match.str(1)) + 1;
        newBaseName = baseName.substr(0, baseName.size() - match.str(1).size()); // Remove the number
    }

    std::string newPath;
    
    // Loop to find a unique file name
    do {
        // Create the new file name
        newPath = getDirectory(originalPath) + newBaseName + std::to_string(newNumber) + extension;
        newNumber++; // Increment for the next check
    } while (fileExists(newPath)); // Check if the new file name already exists

    // Create the new file
	dbgout.open(newPath);
}

#endif