#ifndef SYNC_H
#define SYNC_H

#include <iostream> 
#include <thread> 
#include <mutex> 
#include <queue> 

class Sync{
    void PushLine(std::string Line);
    bool IsLineAvailable(void);
    std::string PopLine(void); 


public:
    std::queue<std::string> Database; 
    std::mutex Padlock; 
public:
    void thread();
    int check();

    bool Isrun = true;
};
#endif  // SYNC_H