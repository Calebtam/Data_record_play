#include "txt_writer.h"

using namespace std;

void DataWriter::mk_multi_dir(const char* dir)
{
	char command[100] = "mkdir -p ";
	strcat(command, dir); 
	
	// 调用系统命令，执行 mkdir -p
	system(command);
} 
bool DataWriter::IsFileExitStat(const string &save_path)
{
   const char* folderPath = save_path.c_str();
   struct stat info;
   if (stat(folderPath, &info) != 0) 
   {
      printf("%s does not exist.\n", folderPath);
      return false;
   } 
   else if (info.st_mode & S_IFDIR) 
   {
      printf("%s exists and is a directory.\n", folderPath);
      return true;
   } 
   else 
   {
      printf("%s exists but is not a directory.\n", folderPath);
      return false;
   }
}
bool DataWriter::IsFileExitAccess(const string &save_path)
{
   std::string result_path = save_path;
   if (access(result_path.c_str(), 0))
   {
      // std::cout << "folder is not exist, create it ..." << std::endl;
      mk_multi_dir(save_path.c_str());
      return false;
   }
   else 
   {
      // std::cout << "folder is exist already, not create it ..." << std::endl;
      return true;
   }
}
bool DataWriter::IsTxtExit(const string &filename)
{
   // ifstream f(filename.c_str());
   // return f.good();
   // // if (f.good())
   // // {
   // //    cout << "文件存在" << endl;
   // // }
   // // else
   // // {
   // //    cout << "文件不存在" << endl;
   // // }

   if (FILE *file = fopen(filename.c_str(), "r")) 
   {
      fclose(file);
      return true;
      //    cout << "文件存在" << endl;
   } 
   else 
   {
      ofstream files;
      files.open (filename, ios::out);
      files << "# new txt " << std::endl;
      files.close();
      return false;
   }   
}

