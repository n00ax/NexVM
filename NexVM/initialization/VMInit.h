#ifndef VMINIT_H
#define VMINIT_H

#include <stdint.h>
#include <string>

#include "NexVM/object/NexObject.h"
#include "NexVM/classloader/ClassLoader.h"
#include "NexVM/interpreter/BaseInterpreter.h"


namespace nexvm{

class VMInit
{
public:
    VMInit();
    int32_t start(int argc, const char **argv);
    void initProcessEntries(int argc, const char **argv);
    void initVM(const std::string &baseLoadClass);
    void VMExecutionLoop();
private:
    void printTitleData();
    std::string loadingClass;
    std::string desiredExecutionPath;
    bool isVerbose, isUnsignedStartup, isUnsignedCodeExecution, isDevelopmentMode;
protected:
    NexObjectDispatch *thisDispatchManager;
    ClassLoader *thisClassLoader;
    BaseInterpreter *thisInterpreter;
    std::vector<std::string> *thisClassLoadingSites;
};

}
#endif // VMINIT_H
