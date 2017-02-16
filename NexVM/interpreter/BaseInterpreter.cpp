#include "BaseInterpreter.h"
#include "BuildConstants.h"
#include "NexVM/exception/ExceptionHandeler.h"

#include <iostream>

namespace nexvm{

struct StandardInstruction{
    uint8_t opcode;
}__attribute__((packed));
struct LoadStoreInstruction{
    uint8_t opcode;
    uint8_t destinationSource;
}__attribute__((packed));
struct WideInstruction{
    uint8_t opcode;
    uint8_t flags; //UNUSED
    uint16_t destinationSource;
}__attribute__((packed));

#define INSTRUCTION_DISPATCH(base)\
    current_instruction_base = current_frame->instructionBase+current_frame->instructionPointer;\
    goto *(instruction_dispatch_table[base]);

//#ifndef INTERPRETER_STEP_MODE
#define INSTRUCTION_CLEANUP(size)\
    INSTRUCTION_DISPATCH((current_frame->instructionBase[current_frame->instructionPointer+=size]));
//#else
//#define INSTRUCTION_CLEANUP(size)\
//    current_frame->instructionPointer += size;\
//    return;
//#endif

#define STACK_PUSH(val)\
    *(current_frame->localStackPointer++) = val;
#define STACK_POP()\
    *(--current_frame->localStackPointer)
#define STACK_PEEK(offset)\
    *(current_frame->localStackPointer[-(offset-1)])
#define STACK_CLEAR()\
    current_frame->localStackPointer = current_frame->localStackFrameBase
#define STACK_TOP()\
    *(current_frame->localStackPointer--)
#define STACK_TOP_PTR()\
    (current_frame->localStackPointer--)
#define STAKCK_GET(pos)\
    *(current_frame->localStackPointer-1-pos)

//Ensure stack-differential is declared before
#define CONSTANT_GET(pos)\
    constantPoolEntryString *str_entry = reinterpret_cast<struct constantPoolEntryString*>\
                                                    (current_frame->constantPoolBase+pos);\
    std::string dest_string = std::string(reinterpret_cast<const char*>(&str_entry->string));

#define OP_PUSH_LOCAL(src)\
    STACK_PUSH(current_frame->localsTableBase[src]);
#define OP_STORE_LOCAL(dest)\
    current_frame->localsTableBase[dest] = STACK_POP();
#define OP_ARITH(op, size)\
    STACK_PUSH((STACK_POP() op STACK_POP()))
#define OP_INT_FETCH_CONST(index, place_command){\
    struct constantPoolEntryInt *int_entry = reinterpret_cast<struct constantPoolEntryInt*>\
                                                            (current_frame->constantPoolBase+index);\
    STACK_PUSH(int_entry->intValue);}
#define OP_STR_GET_SIZE()\
    (reinterpret_cast<struct constantPoolEntryStr*>(current_frame->\
         constantPoolBase[index])->string_size);
#define OP_STR_FETCH_CONST(index, place_command){\
    constantPoolEntryString *str_entry = reinterpret_cast<struct constantPoolEntryString*>\
                                                    (current_frame->constantPoolBase+index);\
    for(uint64_t i=0;i<str_entry->string_size;i++){\
        place_command(str_entry->string[i])\
    }}

#define OPCODE_NOP() OPCODE_NOP: //nop
#define OPCODE_LOAD() OPCODE_LOAD: //load
#define OPCODE_LOAD_0() OPCODE_LOAD_0:
#define OPCODE_LOAD_1() OPCODE_LOAD_1:
#define OPCODE_LOAD_2() OPCODE_LOAD_2:
#define OPCODE_LOAD_3() OPCODE_LOAD_3:
#define OPCODE_WIDE_LOAD() OPCODE_WIDE_LOAD: //loadw
#define OPCODE_STORE() OPCODE_STORE: //store
#define OPCODE_STORE_0() OPCODE_STORE_0:
#define OPCODE_STORE_1() OPCODE_STORE_1:
#define OPCODE_STORE_2() OPCODE_STORE_2:
#define OPCODE_STORE_3() OPCODE_STORE_3:
#define OPCODE_WIDE_STORE() OPCODE_WIDE_STORE: //storew
#define OPCODE_ADD() OPCODE_ADD:
#define OPCODE_FLOAT_ADD() OPCODE_FLOAT_ADD:
#define OPCODE_DOUBLE_ADD() OPCODE_DOUBLE_ADD:
#define OPCODE_SUB() OPCODE_SUB:
#define OPCODE_FLOAT_SUB() OPCODE_FLOAT_SUB:
#define OPCODE_DOUBLE_SUB() OPCODE_DOUBLE_SUB:
#define OPCODE_MUL() OPCODE_MUL:
#define OPCODE_SIGNED_MUL() OPCODE_SIGNED_MUL:
#define OPCODE_FLOAT_MUL() OPCODE_FLOAT_MUL:
#define OPCODE_DOUBLE_MUL() OPCODE_DOUBLE_MUL:
#define OPCODE_DIV() OPCODE_DIV:
#define OPCODE_SIGNED_DIV() OPCODE_SIGNED_DIV:
#define OPCODE_FLOAT_DIV() OPCODE_FLOAT_DIV:
#define OPCODE_DOUBLE_DIV() OPCODE_DOUBLE_DIV:
#define OPCODE_NOT() OPCODE_NOT:
#define OPCODE_AND() OPCODE_AND:
#define OPCODE_OR() OPCODE_OR:
#define OPCODE_XOR() OPCODE_XOR:
#define OPCODE_LEFT_SHIFT() OPCODE_LEFT_SHIFT:
#define OPCODE_RIGHT_SHIFT() OPCODE_RIGHT_SHIFT:
#define OPCODE_JMP() OPCODE_JMP:
#define OPCODE_LOAD_CONSTANT() OPCODE_LOAD_CONSTANT:
#define OPCODE_NEW() OPCODE_NEW:
#define OPCODE_GET_FIELD_REF() OPCODE_GET_FIELD_REF:
#define OPCODE_GET_METHOD_REF() OPCODE_GET_METHOD_REF:
#define OPCODE_LOAD_FIELD() OPCODE_LOAD_FIELD:
#define OPCODE_WRITE_FIELD_BYTE() OPCODE_WRITE_FIELD_BYTE:
#define OPCODE_WRITE_FIELD_WORD() OPCODE_WRITE_FIELD_WORD:
#define OPCODE_WRITE_FIELD_DWORD() OPCODE_WRITE_FIELD_DWORD:
#define OPCODE_WRITE_FIELD_QWORD() OPCODE_WRITE_FIELD_QWORD:
#define OPCODE_WRITE_FIELD_ARR_BYTE() OPCODE_WRITE_FIELD_ARR_BYTE:
#define OPCODE_WRITE_FIELD_ARR_WORD() OPCODE_WRITE_FIELD_ARR_WORD:
#define OPCODE_WRITE_FIELD_ARR_DWORD() OPCODE_WRITE_FIELD_ARR_DWORD:
#define OPCODE_WRITE_FIELD_ARR_QWORD() OPCODE_WRITE_FIELD_ARR_QWORD:
#define OPCODE_INVOKE() OPCODE_INVOKE:
#define OPCODE_INVOKE_NATIVE() OPCODE_INVOKE_NATIVE:
#define OPCODE_END_STREAM() OPCODE_END_STREAM:
#define OPCODE_GOTO() OPCODE_GOTO:
#define OPCODE_UNUSED_() OPCODE_UNUSED_:

BaseInterpreter::BaseInterpreter(){

}
ExitState BaseInterpreter::interpretInstruction(NexExecutionFrame *current_frame,
                                           nexvm::NexObjectDispatch *dispatchFrame,
                                           ClassLoader *loaderInstance){
    uint8_t *current_instruction_base;
    void *instruction_dispatch_table[256] = {
        &&OPCODE_NOP, &&OPCODE_LOAD,  &&OPCODE_WIDE_LOAD, &&OPCODE_LOAD_0, &&OPCODE_LOAD_1, &&OPCODE_LOAD_2,
        &&OPCODE_LOAD_3, &&OPCODE_STORE, &&OPCODE_WIDE_STORE,
        &&OPCODE_STORE_0, &&OPCODE_STORE_1, &&OPCODE_STORE_2, &&OPCODE_STORE_3,
        &&OPCODE_ADD, &&OPCODE_FLOAT_ADD, &&OPCODE_DOUBLE_ADD,
        &&OPCODE_SUB, &&OPCODE_FLOAT_SUB, &&OPCODE_DOUBLE_SUB, &&OPCODE_MUL, &&OPCODE_SIGNED_MUL,
        &&OPCODE_FLOAT_MUL, &&OPCODE_DOUBLE_MUL, &&OPCODE_DIV, &&OPCODE_SIGNED_DIV,
        &&OPCODE_FLOAT_DIV, &&OPCODE_DOUBLE_DIV, &&OPCODE_NOT, &&OPCODE_AND, &&OPCODE_OR,
        &&OPCODE_XOR, &&OPCODE_LEFT_SHIFT, &&OPCODE_RIGHT_SHIFT,
        &&OPCODE_LOAD_CONSTANT, &&OPCODE_NEW, &&OPCODE_GET_FIELD_REF, &&OPCODE_GET_METHOD_REF,
        &&OPCODE_LOAD_FIELD,
        &&OPCODE_WRITE_FIELD_BYTE, &&OPCODE_WRITE_FIELD_WORD, &&OPCODE_WRITE_FIELD_DWORD,
        &&OPCODE_WRITE_FIELD_QWORD, &&OPCODE_WRITE_FIELD_ARR_BYTE, &&OPCODE_WRITE_FIELD_ARR_WORD,
        &&OPCODE_WRITE_FIELD_ARR_DWORD, &&OPCODE_WRITE_FIELD_ARR_QWORD, &&OPCODE_INVOKE,
        &&OPCODE_INVOKE_NATIVE, &&OPCODE_END_STREAM, &&OPCODE_GOTO, [49 ... 255] = &&OPCODE_UNUSED_
    };
    INSTRUCTION_DISPATCH(current_frame->instructionBase[current_frame->instructionPointer]){
        OPCODE_NOP()
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_LOAD()
                //OP_PUSH_LOCAL(reinterpret_cast<struct LoadStoreInstruction*>(current_instruction_base)->destinationSource)
                STACK_PUSH(current_frame->localsTableBase[reinterpret_cast<struct LoadStoreInstruction*>(current_instruction_base)->destinationSource]);
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_WIDE_LOAD()
                OP_PUSH_LOCAL(reinterpret_cast<struct WideInstruction*>(current_instruction_base)->destinationSource)
                INSTRUCTION_CLEANUP(sizeof(struct WideInstruction))
        OPCODE_LOAD_0()
                OP_PUSH_LOCAL(0)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_LOAD_1()
                OP_PUSH_LOCAL(1)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_LOAD_2()
                OP_PUSH_LOCAL(2)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_LOAD_3()
                OP_PUSH_LOCAL(3)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_STORE()
                OP_STORE_LOCAL(reinterpret_cast<struct LoadStoreInstruction*>(current_instruction_base)->destinationSource)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction));
        OPCODE_WIDE_STORE()
                OP_STORE_LOCAL(reinterpret_cast<struct WideInstruction*>(current_instruction_base)->destinationSource)
                INSTRUCTION_CLEANUP(sizeof(struct WideInstruction))
        OPCODE_STORE_0()
                OP_STORE_LOCAL(0)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_STORE_1()
                OP_STORE_LOCAL(1)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_STORE_2()
                OP_STORE_LOCAL(2)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_STORE_3()
                OP_STORE_LOCAL(3)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_ADD()
                OP_ARITH(+, uint64_t)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_FLOAT_ADD()
                OP_ARITH(+, float)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_DOUBLE_ADD()
                OP_ARITH(+, double)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_SUB()
                OP_ARITH(-, uint64_t)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_FLOAT_SUB()
                OP_ARITH(-, float)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_DOUBLE_SUB()
                OP_ARITH(-, double)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_MUL()
                OP_ARITH(*, uint64_t)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_SIGNED_MUL()
                OP_ARITH(*, int64_t)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_FLOAT_MUL()
                OP_ARITH(*, float)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_DOUBLE_MUL()
                OP_ARITH(*, double)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_DIV()
                OP_ARITH(/, uint64_t)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_SIGNED_DIV()
                OP_ARITH(/, int64_t)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_FLOAT_DIV()
                OP_ARITH(/, float)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_DOUBLE_DIV()
                OP_ARITH(/, double)
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_NOT()
                STACK_PUSH(!(STACK_POP()))
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_AND()
                STACK_PUSH(STACK_POP() & STACK_POP())
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_OR()
                STACK_PUSH(STACK_POP() | STACK_POP())
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_XOR()
                STACK_PUSH(STACK_POP() ^ STACK_POP())
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_LEFT_SHIFT()
                STACK_PUSH(STACK_POP() << STACK_POP())
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_RIGHT_SHIFT()
                STACK_PUSH(STACK_POP() >> STACK_POP())
                INSTRUCTION_CLEANUP(sizeof(struct StandardInstruction))
        OPCODE_LOAD_CONSTANT()
                switch(reinterpret_cast<struct constantPoolEntryInt*>(current_frame->constantPoolBase +
                                                                      reinterpret_cast<struct WideInstruction*>
                (current_instruction_base)->destinationSource)->tag){
                case ENTRY_STRING:
                OP_STR_FETCH_CONST(reinterpret_cast<struct WideInstruction*>
                                   (current_instruction_base)->destinationSource, STACK_PUSH)
                break;
                case ENTRY_INT:
                OP_INT_FETCH_CONST(reinterpret_cast<struct WideInstruction*>
                                   (current_instruction_base)->destinationSource, STACK_PUSH)
                break;
        }
        OPCODE_NEW(){
                CONSTANT_GET(reinterpret_cast<struct WideInstruction*>(current_instruction_base)->destinationSource)
                STACK_PUSH((dispatchFrame)->registerNexObject(loaderInstance->getClass(dest_string)));
        }
                INSTRUCTION_CLEANUP(sizeof(struct WideInstruction))
        OPCODE_GET_FIELD_REF(){
                CONSTANT_GET(reinterpret_cast<struct WideInstruction*>(current_instruction_base)->destinationSource)
                STACK_PUSH(dispatchFrame->getObject(STACK_POP())->getStaticData()->getFieldReference(dest_string));
        }
                INSTRUCTION_CLEANUP(sizeof(struct WideInstruction))
        OPCODE_GET_METHOD_REF(){
                CONSTANT_GET(reinterpret_cast<struct WideInstruction*>(current_instruction_base)->destinationSource)
                STACK_PUSH(dispatchFrame->getObject(STACK_POP())->getStaticData()->getMethodReference(dest_string));
        }
                INSTRUCTION_CLEANUP(sizeof(struct WideInstruction));
        OPCODE_LOAD_FIELD(){
            /*
             * Precondition = SP=Object ID, SP-1=Field ID, Postcondition, SP-1=Value
             */
                STACK_PUSH(dispatchFrame->getObject(STACK_POP())->getField(current_frame->isVerified,
                                                                           current_frame->thisFrameClassID, STACK_POP()))
        }
        OPCODE_WRITE_FIELD_BYTE(){
            /*
             * Precondition = SP=Object ID, SP-1=Field ID, SP-2=Value, Postcondition = None
             */
                dispatchFrame->getObject(STACK_POP())->modField(current_frame->isVerified,
                                                                current_frame->thisFrameClassID, STACK_POP(),
                            static_cast<uint8_t>(STACK_POP()));
        }
        OPCODE_WRITE_FIELD_WORD(){
                dispatchFrame->getObject(STACK_POP())->modField(current_frame->isVerified,
                                                                current_frame->thisFrameClassID, STACK_POP(),
                            static_cast<uint16_t>(STACK_POP()));
        }
        OPCODE_WRITE_FIELD_DWORD(){
                dispatchFrame->getObject(STACK_POP())->modField(current_frame->isVerified,
                                                                current_frame->thisFrameClassID, STACK_POP(),
                            static_cast<uint32_t>(STACK_POP()));
        }
        OPCODE_WRITE_FIELD_QWORD(){
                dispatchFrame->getObject(STACK_POP())->modField(current_frame->isVerified,
                                                                current_frame->thisFrameClassID, STACK_POP(),
                            static_cast<uint64_t>(STACK_POP()));
        }
        OPCODE_WRITE_FIELD_ARR_BYTE(){
                dispatchFrame->getObject(STACK_POP())->modField(current_frame->isVerified,
                                                                current_frame->thisFrameClassID, STACK_POP(), STACK_POP(),
                                                                static_cast<uint8_t>(STACK_POP()));
        }
        OPCODE_WRITE_FIELD_ARR_WORD(){
                dispatchFrame->getObject(STACK_POP())->modField(current_frame->isVerified,
                                                                current_frame->thisFrameClassID, STACK_POP(), STACK_POP(),
                                                                static_cast<uint16_t>(STACK_POP()));
        }
        OPCODE_WRITE_FIELD_ARR_DWORD(){
                dispatchFrame->getObject(STACK_POP())->modField(current_frame->isVerified,
                                                                current_frame->thisFrameClassID, STACK_POP(), STACK_POP(),
                                                                static_cast<uint32_t>(STACK_POP()));
        }
        OPCODE_WRITE_FIELD_ARR_QWORD(){
                dispatchFrame->getObject(STACK_POP())->modField(current_frame->isVerified,
                                                                current_frame->thisFrameClassID, STACK_POP(), STACK_POP(),
                                                                static_cast<uint64_t>(STACK_POP()));
        }
        OPCODE_INVOKE(){
                NexExecutionFrame *newFrame = NULL;
                dispatchFrame->PushExecutionFrame(current_frame);
        }
        OPCODE_INVOKE_NATIVE(){

        }
        OPCODE_END_STREAM() //47
        {
            return EXIT__;
        }
        OPCODE_GOTO(){
            /*std::cout << "D" << std::endl;
            uint64_t frame_offset = STACK_POP();
            if(frame_offset > dispatchFrame->getObject(current_frame->thisFrameObjectID)->getStaticData()->
                    getMethod(current_frame->thisFrameMethodID)->getMethodSize()){
                nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_GOTO_TARGET, std::to_string(frame_offset));
            }
            else{
                current_frame->instructionPointer += frame_offset;
            }
            */
        }
        OPCODE_UNUSED_()
        {
            return EXIT_UNUSED_OPCODE;
        }
   }
}
inline void BaseInterpreter::objectAllocationStub(){

}
inline void BaseInterpreter::objectCallingStub(){

}
inline void BaseInterpreter::objectStaticCallingStub(){

}
inline void BaseInterpreter::objectSpecialCallingStub(){

}
}
