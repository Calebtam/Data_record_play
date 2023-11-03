#include "Sync.h"

void Sync::PushLine(std::string Line) { 
    std::unique_lock<std::mutex> Lock(Padlock); (void)Lock; 
    Database.push(Line); 
} 
bool Sync::IsLineAvailable(void) { 
    std::unique_lock<std::mutex> Lock(Padlock); (void)Lock; 
    return !Database.empty(); 
} 
std::string Sync::PopLine(void) { 
    std::unique_lock<std::mutex> Lock(Padlock); (void)Lock; 
    std::string Line(std::move(Database.back())); 
    Database.pop(); 
    return Line; 
} 
void Sync::thread()
{ 
    do { 
        // Ensure the input is as clean as possible 
        if (std::cin.rdbuf()->in_avail()) { 
            std::cin.ignore(std::cin.rdbuf()->in_avail()); 
        } 
        std::cin.clear(); 

        // Get a line, cin will block here. 
        std::string Line; 
        std::getline(std::cin, Line); 

        // If the line is not empty attempt to store it. 
        if (!Line.empty()) { 
            PushLine(Line); 
        } 
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    } while (1); 
}
int Sync::check()
{
    // Check for available input 
    if (IsLineAvailable()) { 
        // If there is input available, first get it 
        std::string Line = PopLine(); 

        // Echo it to the terminal 
        std::cout << "Command: " << Line << std::endl; 

        // Perform actions based on the command 
        if (Line == "quit") { 
            // Running = false; 
            std::cout << "stop: " << Line << std::endl; 
            Isrun = false;
            return -1;
        } 
    }
    if(!Isrun)
    {
        return -1;
    }
    return 1;
} 