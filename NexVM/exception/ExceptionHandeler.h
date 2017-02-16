#ifndef EXCEPTIONHANDELER_H
#define EXCEPTIONHANDELER_H

#include <string>
#include <iostream>
#include <stdlib.h>

namespace nexvm{

enum builtInExceptionType{
    VM_STACK_OVERFLOW, VM_MEMORY_ALLOCATION_ERR, VM_NON_EXISTANT_INSTRUCTION,
    VM_MALFORMED_CLASS, VM_CLASS_NOT_FOUND, VM_INVALID_FIELD_WRITE_SIZE,
    VM_INVALID_FIELD_SIZE, VM_INVALID_FIELD_ARR_NO_SIZE, VM_INVALID_FIELD_NONARR_SIZE,
    VM_ARR_FIELD_OUT_OF_BOUNDS, VM_INVALID_METHOD_RESOLUTION_NAME_NULL,
    VM_INVALID_CONSTANT_OFFSET, VM_MALFORMED_HEADER_FILE_TOO_SMALL,
    VM_HEADER_MAGIC_OR_SIZE_INVALID, VM_FIELDS_OVERFLOW, VM_INVALID_FIELD_WRITE_OFFSET,
    VM_OBJECT_DOES_NOT_EXIST, VM_FIELD_DOES_NOT_EXIST, VM_PRIVATE_ACCESS_VIOLATION,
    VM_PROTECTED_ACCESS_VIOLATION, VM_INVALID_INSTRUCTION_OPCODE, VM_INVALID_GOTO_TARGET
};
const std::string builtInExceptionVectors[] = {
    "VM.StackOverflow", "VM.MemoryAllocationError", "VM.NonExistantInstruction",
    "VM.MalformedClass", "VM.ClassNotFound", "VM.InvalidFieldWriteSize",
    "VM.InvalidFieldSize", "VM.InvalidFieldSpecifiedArrNoSize", "VM.InvalidFieldNotArrHasSize",
    "VM.ArrFieldOutOfBounds", "VM.InvalidMethodResolutionName<NULL>", "VM.InvalidConstantOffset",
    "VM.MalformedHeaderFileTooSmall", "VM.HeaderMagicOrVersionInvalid",
    "VM.FieldsOverflowDescriptorTooLarge", "VM.InvalidFieldWriteOffset",
    "VM.ObjectDoesNotExist", "VM.FieldDoesNotExist", "VM.PrivateAccessViolation",
    "VM.ProtectedAccessViolation", "VM.InvalidInstructionOpcode" "VM.InvalidGotoTarget"
};

class ExceptionHandeler
{
public:
    ExceptionHandeler();
    static void handleInternalState(enum builtInExceptionType exception){
        std::cerr << "Unexpected Internal State Exception Has Arisen: \n\t" <<
                     builtInExceptionVectors[exception] << std::endl;
        exit(-1);
    }
    static void handleInternalState(enum builtInExceptionType exception,
                                    const std::string &extraString){
        std::cerr << "Unexpected Internal State Exception Has Arisen: \n\t" <<
                     builtInExceptionVectors[exception] << "-->" << extraString << std::endl;
        exit(-1);
    }
};


}

#endif // EXCEPTIONHANDELER_H
