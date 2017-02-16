#ifndef BASEINTERPRETER_H
#define BASEINTERPRETER_H

#include <stdint.h>
#include "NexVM/object/ClassDefinitions.h"
#include "NexVM/object/NexObject.h"
#include "NexVM/classloader/ClassLoader.h"

namespace nexvm{

/*
 * Wide is indicitive of word aligned accesses for larger local sizes;
 * Otherwise the local variable is assumed to be within one bytes range
 */
enum opcodeDefinitions{
    OPCODE_NOP, OPCODE_LOAD, OPCODE_WIDE_LOAD, OPCODE_LOAD_0, OPCODE_LOAD_1, OPCODE_LOAD_2,
    OPCODE_LOAD_3, OPCODE_STORE, OPCODE_WIDE_STORE, OPCODE_STORE_0, OPCODE_STORE_1, OPCODE_STORE_2,
    OPCODE_STORE_3, OPCODE_ADD, OPCODE_FLOAT_ADD, OPCODE_DOUBLE_ADD, OPCODE_SUB,
    OPCODE_FLOAT_SUB, OPCODE_DOUBLE_SUB, OPCODE_MUL, OPCODE_SIGNED_MUL,
    OPCODE_FLOAT_MUL, OPCODE_DOUBLE_MUL, OPCODE_DIV, OPCODE_SIGNED_DIV,
    OPCODE_FLOAT_DIV, OPCODE_DOUBLE_DIV, OPCODE_NOT, OPCODE_AND, OPCODE_OR, OPCODE_XOR,
    OPCODE_LEFT_SHIFT, OPCODE_RIGHT_SHIFT, OPCODE_LOAD_CONSTANT,  OPCODE_NEW, OPCODE_GET_FIELD_REF,
    OPCODE_GET_METHOD_REF, OPCODE_LOAD_FIELD,
    OPCODE_WRITE_FIELD_BYTE, OPCODE_WRITE_FIELD_WORD, OPCODE_WRITE_FIELD_DWORD, OPCODE_WRITE_FIELD_QWORD,
    OPCODE_WRITE_FIELD_ARR_BYTE, OPCODE_WRITE_FIELD_ARR_WORD, OPCODE_WRITE_FIELD_ARR_DWORD,
    OPCODE_WRITE_FIELD_ARR_QWORD, OPCODE_INVOKE, OPCODE_INVOKE_NATIVE, OPCODE_END_STREAM,
    OPCODE_GOTO, OPCODE_UNUSED_
};
enum ExitState{
    EXIT__, EXIT_INVOKE, EXIT_UNUSED_OPCODE
};

struct fieldDescriptor{

};

class BaseInterpreter
{
public:
    BaseInterpreter();
    ExitState interpretInstruction(NexExecutionFrame *current_frame, nexvm::NexObjectDispatch *dispatchFrame,
                              ClassLoader *loaderInstance);
private:
    void objectAllocationStub();
    void objectCallingStub();
    void objectStaticCallingStub();
    void objectSpecialCallingStub();
};

}
#endif //BASEINTERPRETER_H
