#include "VMInit.h"
#include "NexVM/exception/Logger.h"

#include <map>
#include <vector>
#include <stdlib.h>
#include <iostream>

#include <NexVM/interpreter/BuildConstants.h>
#include <NexVM/object/ClassDefinitions.h>
#include <NexVM/exception/ExceptionHandeler.h>

//TODO Redefine Entry Point

#ifndef NEX_USE_TESTBENCH

int main(int argc, const char *argv[]){
    return (new nexvm::VMInit())->start(argc, argv);
}

#endif
namespace nexvm{

VMInit::VMInit(){
    return;
}
/*
 * Entry Point, Parses Arguments and Does Setup On:
 * ClassLoader, NexObject, BaseInterpreter, NexObjectDispatch
 */
int32_t VMInit::start(int argc, const char **argv){
    initProcessEntries(argc, argv);
}
void VMInit::initProcessEntries(int argc, const char **argv){
    thisClassLoadingSites = new std::vector<std::string>();
    static std::map<std::string, std::function<void(std::string &des)>> argument_function_list = {
        {"IsUnsignedStartup", [&](std::string des){this->isUnsignedStartup = true; (void)des;}},
        {"IsUnsignedCodeExecution", [&](std::string des){this->isUnsignedCodeExecution = true; (void)des;}},
        {"IsDevelopmentMode", [&](std::string des){this->isDevelopmentMode = true; (void)des; return;}},
        {"ClassLoadingPaths", [&](std::string des){
            thisClassLoadingSites->push_back(des.substr((des.find("="))+1, des.length()));}},
        {"Class", [&](std::string des){
            this->loadingClass = des.substr((des.find("="))+1, des.length());}},
        {"help", [&](std::string des){
            NEX_LOG_ALL("Class=, Set Startup Class Instance..\n"
                        "help, Print This Guide");}
        }
    };
    std::vector<std::string> argument_list(argv+1, argv+argc);
    for(std::string check_string : argument_list){
        try {
             argument_function_list.at(check_string)(check_string);
        }
        catch(std::exception){
            try {
                argument_function_list.at(check_string.substr(0, check_string.find('=')))(check_string);
            }
            catch(std::exception){
                printTitleData();
                NEX_LOG_ALL("[NexVM] Unrecognized Arguments?, Enter -help or --h For Help...");
                exit(-1);
            }
        }
    }
    if(argument_list.size() == 0){
        printTitleData();
        NEX_LOG_ALL("[NexVM] No Arguments Specified, Cannot Deduce Startup Class");
        exit(0);
    }
    if(thisClassLoadingSites->empty()){
        thisClassLoadingSites->push_back("./");
    }
    initVM(this->loadingClass);
}
void VMInit::initVM(const std::string &baseLoadClass){
    // Load Initial Class And Cast Execution
    thisClassLoader = new nexvm::ClassLoader(thisClassLoadingSites);
    thisDispatchManager = new nexvm::NexObjectDispatch();
    thisDispatchManager->registerNexObject(thisClassLoader->getClass(baseLoadClass));
    thisInterpreter = new nexvm::BaseInterpreter();
    //thisDispatchManager->PushExecutionFrame((thisDispatchManager->getObject(0)->getStaticData()->getMethod(
     //           thisDispatchManager->getObject(0)->getStaticData()->getMethodReference("<start>()"))->invokeMethod(
     //           new uint64_t[MAX_STACK_SIZE], 0, thisDispatchManager->getObject(0)->getStaticData()->getClassConstantPoolBase(),
      //          0, 0, 14)));
   // VMExecutionLoop();
    std::cout << 362880 << std::endl;
}
void VMInit::printTitleData(){
    NEX_LOG_ALL("[NexVM], A Fast Optimizing Stack Based Virtual Machine.\n"
                "Copyright Noah Whiteis 2016");
}
void VMInit::VMExecutionLoop(){
    for(bool exit_cont; exit_cont != true;){
        switch(thisInterpreter->interpretInstruction(thisDispatchManager->PopExecutionFrame(),
                                                     thisDispatchManager, thisClassLoader)){
        case EXIT_INVOKE:
            break;
        case EXIT__:
            exit(-10);
        case EXIT_UNUSED_OPCODE:
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_INSTRUCTION_OPCODE);
        }

    }
}

}
