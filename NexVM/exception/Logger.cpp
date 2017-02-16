#include "Logger.h"

#include <iostream>

namespace nexvm{

void logInternalDoNotExpose(const char *message){
    std::cout << "[Verbose] " << message << std::endl;
}
void logInternalDoNotExposeNoVerbose(const char *message){
    std::cout << message << std::endl;
}

}
