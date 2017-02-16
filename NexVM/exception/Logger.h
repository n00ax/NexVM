#ifndef LOGGER_H
#define LOGGER_H

#include <string>

#include "NexVM/interpreter/BuildConstants.h"

namespace nexvm{

void logInternalDoNotExpose(const char *message);
void logInternalDoNotExposeNoVerbose(const char *message);

#ifdef USE_VERBOSE
#define NEX_LOG(message)\
    nexvm::logInternalDoNotExpose(message);
#endif
#ifndef USE_VERBOSE
#define NEX_LOG(message)
#endif

#define NEX_LOG_ALL(message)\
    nexvm::logInternalDoNotExposeNoVerbose(message);
#define NEX_LOG_DEBUG(message)\
    nexvm::logInternalDoNotExpose(message)

}


#endif // LOGGER_H
