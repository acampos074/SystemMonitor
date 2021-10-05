#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    
    long HH,MM;
    MM = seconds/60;
    HH = MM/60;

    return std::to_string(HH)+":"+std::to_string(MM%60)+":"+std::to_string(seconds%60); 
    
}