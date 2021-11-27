/* Created by Jacob Chrisitan and Levi Jacoby */


#ifndef ResultsLogger_h
#define ResultsLogger_h

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>


class ResultsLogger {
    private:
        std::string fileName;
        std::fstream fileStream;
        std::string CreateDefaultFileName();
    public:
        ResultsLogger(std::string fileName);
        void OpenFile();
        void CloseFile();
        bool FileIsOpen();
        void WriteToFile(std::string outStream);
        void ReadFromFile();
};

#endif /* ResultsLogger_h */
