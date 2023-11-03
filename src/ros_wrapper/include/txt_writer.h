#ifndef TXT_WRITER_H
#define TXT_WRITER_H

#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

// creat new file
#include <dirent.h>
#include <fcntl.h>
#include <sys/param.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

using namespace std;

class DataWriter
{
public:
    DataWriter(const string Path, const int catagory){

        IsFileExitAccess(Path);

        for(int i = 1; i <= catagory; i++)
        {
            stringstream stream;
            stream << i;
            string result;
            stream >> result;           

            std::string newTxt = Path + "/" + result + ".txt";
            IsTxtExit(newTxt);

            DataSavePath.push_back(newTxt);
        }        
        // std::cout << DataSavePath.size() << std::endl;
    }
    void mk_multi_dir(const char* dir);
    bool IsFileExitStat(const string &save_path);
    bool IsFileExitAccess(const string &save_path);
    bool IsTxtExit(const string &filename);

    // void WriterImg(const cv::Mat &img_msg);
    // void WriterImg(const cv::Mat &img_msg1, const cv::Mat &img_msg2);
    std::vector<string> DataSavePath;
    long factory_id = 0;
};


#endif  // TXT_WRITER_H