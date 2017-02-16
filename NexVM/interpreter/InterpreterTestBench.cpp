#include "InterpreterTestBench.h"
#include "BaseInterpreter.h"
#include "NexVM/object/ClassDefinitions.h"
#include "NexVM/object/NexObject.h"
#include "NexVM/classloader/ClassLoader.h"
#include "NexVM/interpreter/BuildConstants.h"
#include <iostream>
#include <vector>

#ifdef NEX_USE_TESTBENCH

int main(){
    InterpreterTestBench::InterpreterTestBench();
}

#endif

InterpreterTestBench::InterpreterTestBench()
{
    nexvm::ClassLoader *loaderClass = new nexvm::ClassLoader(new std::vector<std::string>());
    nexvm::NexClass *testClass = loaderClass->getClass("test.class");
    //std::cout << (uint32_t*)*testClass->getMethodSpec(0)->thisMethodBase << std::endl;
    nexvm::NexObjectDispatch *obd = new nexvm::NexObjectDispatch();
    obd->registerNexObject(testClass);
    nexvm::NexExecutionFrame s =  nexvm::NexExecutionFrame();
    uint64_t stack[] = {
        9, 2, 2, 5, 6, 7, 9, 2,0,0,0,0,0,0,0,0,0,0,0
    };
    //nexvm::NexExecutionFrame *d = obd->getObject(0)->getStaticData()->getMethod(0)->invokeMethod(
              //  stack+8, 4, new uint8_t[30], 1, 1, 8);
   // std::cout << (uint32_t)*(d->instructionBase) << std::endl;
    //nexvm::BaseInterpreter *new_base = new nexvm::BaseInterpreter();
    //new_base->interpretInstruction(d, obd, loaderClass);
    // std::cout << testClass->getClassName() << std::endl;
    //std::cout << testClass->getMethodSpec(0)->thisMethodName << std::endl;
    /*
    nexvm::BaseInterpreter *interpInstance = new nexvm::BaseInterpreter;
    nexvm::NexExecutionFrame *execFrame = new nexvm::NexExecutionFrame();
    execFrame->constantPoolBase = (reinterpret_cast<uint8_t*>(new struct nexvm::constantPoolEntryInt()));
    (reinterpret_cast<struct nexvm::constantPoolEntryString*>(execFrame->constantPoolBase))->string[0] = 'H';
    (reinterpret_cast<struct nexvm::constantPoolEntryString*>(execFrame->constantPoolBase))->string[1] = 'i';
    (reinterpret_cast<struct nexvm::constantPoolEntryString*>(execFrame->constantPoolBase))->string[2] = '!';
    (reinterpret_cast<struct nexvm::constantPoolEntryString*>(execFrame->constantPoolBase))->tag = nexvm::ENTRY_STRING;
    (reinterpret_cast<struct nexvm::constantPoolEntryString*>(execFrame->constantPoolBase))->string_size = 3;
    execFrame->localsTableBase = new uint64_t[20];
    execFrame->localStackPointer = new uint64_t[20];
    execFrame->localStackFrameBase = execFrame->localStackPointer;
    *(execFrame->localStackPointer++) = (double)3.1;
    *(execFrame->localStackPointer++) = (double)3.1;
    uint8_t opcodeSingle[10] = {nexvm::OPCODE_DOUBLE_ADD};
    for(int i=0;i<10;i++){
        if(execFrame->localStackPointer == execFrame->localStackFrameBase+i){
            std::cout << "SP==>";
        }
        std::cout << "Begin:" << execFrame->localStackFrameBase+i << std::endl;
         std::cout << "Value:" << execFrame->localStackFrameBase[i] << std::endl;
          std::cout << "End:" << execFrame->localStackFrameBase+i << std::endl;
    }
    std::cout << "=================================" << std::endl;
    for(int i=0;i<10;i++){
        if(execFrame->localStackPointer == execFrame->localStackFrameBase+i){
            std::cout << "SP==>";
        }
        std::cout << "Begin:" << execFrame->localStackFrameBase+i << std::endl;
         std::cout << "Value:" << (double)execFrame->localStackFrameBase[i] << std::endl;
          std::cout << "End:" << execFrame->localStackFrameBase+i+1 << std::endl;

    }
    */
}
