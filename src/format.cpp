#include <string>
#include <iomanip>
#include "format.h"


using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    
    long HH;
    long MM;
    MM = seconds/60;
    HH = MM/60;

    std::ostringstream stream;
    string hour;
    string min;
    string sec;
    hour = std::to_string(HH);
    min = std::to_string(MM%60);
    sec = std::to_string(seconds%60); 
    
    stream << std::setw(2) << std::setfill('0') << hour << ":" 
    << std::setw(2) << std::setfill('0') << min << ":"
    << std::setw(2) << std::setfill('0') << sec;

    return stream.str();
}