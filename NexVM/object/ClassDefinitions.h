#ifndef CLASSDEFINITIONS_H
#define CLASSDEFINITIONS_H

#include <stdint.h>
#include <vector>

namespace nexvm{

union NexFieldAccessFlags;

typedef uint64_t ClassID;
typedef uint64_t MethodID;
typedef uint64_t ObjectID;

class NexExecutionFrame{
public:
    ~NexExecutionFrame(){
        delete localsTableBase;
        delete localStackFrameBase;
    }
    uint8_t *constantPoolBase, *instructionBase;
    uint64_t *localsTableBase;
    uint64_t instructionPointer, *localStackFrameBase, *localStackPointer,
    localStackSize, localsTableSize;
    ClassID thisFrameClassID; //ClassID alias to prevent circular dependencies
    ObjectID thisFrameObjectID;
    MethodID thisFrameMethodID;
    bool isVerified;
};

enum constantPoolEntryTags{
    ENTRY_STRING, ENTRY_INT, ENTRY_METHOD_REF, ENTRY_CLASS_REF, ENTRY_OBJECT_REF
};

struct constantPoolEntryInt{
    uint8_t tag;
    uint64_t intValue;
}__attribute__ ((packed));
struct constantPoolEntryString{
    uint8_t tag;
    uint64_t string_size;
    uint8_t string[0];
}__attribute__ ((packed));;

}

#endif // CLASSDEFINITIONS_H
