/* Created by Jacob Chrisitan and Levi Jacoby */


#include "ResultsLogger.h"

ResultsLogger::ResultsLogger(std::string fileName) {
    this->fileName = fileName;
}

std::string ResultsLogger::CreateDefaultFileName(){
    return "project-3-results.csv";
}

void ResultsLogger::OpenFile() {
    this->fileStream.open(this->fileName, std::fstream::in | std::fstream::out | std::fstream::trunc);
}

void ResultsLogger::CloseFile() {
    this->fileStream.close();
}

bool ResultsLogger::FileIsOpen() {
    return fileStream.is_open();
}

void ResultsLogger::WriteToFile(std::string outStream) {
    if (ResultsLogger::FileIsOpen()) {
        //if new line character is not found at end of string, add it
        if (outStream.find("\n") == std::string::npos) {
            outStream += "\n";
        }
        this->fileStream << outStream;
    }
}

void ResultsLogger::ReadFromFile() {
    if (ResultsLogger::FileIsOpen()) {
        //return to the beginning of the file
        fileStream.clear();
        fileStream.seekg(0);
        std::string currentLine;
        while (std::getline(this->fileStream, currentLine)) {
            std::cout << currentLine << "\n";
        }
        //return to the beginning of the file
        fileStream.clear();
        fileStream.seekg(0);
    }
}
