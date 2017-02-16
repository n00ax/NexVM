#ifndef NEXOBJECT_H
#define NEXOBJECT_H

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <string>

#include <NexVM/object/ClassDefinitions.h>

namespace nexvm{

class NexObject;
class NexObjectStaticData;
struct InterfaceSpec;
struct MethodSpec;
struct FieldSpec;

typedef uint64_t FieldID, MethodID, InterfaceID, ObjectID, ClassID;

enum FieldType : uint64_t{
    FIELD_ARRAY_BYTES, FIELD_ARRAY_WORDS, FIELD_ARRAY_DWORDS, FIELD_ARRAY_QWORDS,
    BYTE, WORD, DWORD, QWORD
};
/*
 * [Future]
 */
enum AccessType{
    IS_PUBLIC, IS_PRIVATE, IS_PROTECTED
};

struct FieldDataBlock{
        AccessType fieldAccessType : 2;
        FieldType fieldAccessSize : 4;
        uint8_t isSynchronized : 1;
        uint64_t fieldOffset;
        uint64_t arraySize;
};

/*
 * [End]
 */

typedef uint8_t NexField;

class NexMethod{
public:
    NexMethod(uint8_t *destMethodBytecodeBase, uint64_t destMethodSize, AccessType destMethodAccessType);
    nexvm::NexExecutionFrame* invokeMethod(uint64_t *stackArgumentBase, uint64_t stackArgumentCount,
                                           uint8_t *newConstantPoolBase, ObjectID thisObjectListId,
                                           ClassID thisObjectClassID, uint64_t oldStackArgumentTop,
                                           ClassID callingClass, NexObjectStaticData *thisCalleeData);
    uint64_t getMethodSize();
    AccessType getMethodAccessType();
    void checkMethodPermissions(ClassID thisClass, ClassID callingClass, AccessType thisAccessType);
private:
    AccessType methodAccessType;
    uint64_t methodSize;
    uint8_t *methodBytecodeBase;
    ClassID thisClassID;
};
class NexInterface{
public:
    NexInterface(InterfaceSpec *interfacePrototype);
    InterfaceSpec *thisInterface;
};
/*
 * "Specs" define object field attributes
 */
struct FieldSpec{
public:
    FieldSpec(const std::string &fieldName, enum FieldType desFieldType,
              uint64_t fieldSize, AccessType fieldAccess);
    std::string thisFieldName;
    enum AccessType thisFieldAccessType;
    uint64_t fieldSize; // For Arrays
    enum FieldType thisFieldType;
    bool isSynchronized;
};
struct MethodSpec{
    MethodSpec(const std::string &methodName, uint8_t *methodBase, uint64_t methodSize,
               AccessType thisMethodAccessType);
    std::string thisMethodName;
    uint64_t thisMethodSize;
    uint8_t *thisMethodBase;
    AccessType thisMethodAccessType;
};
struct InterfaceSpec{
    InterfaceSpec(const std::string &interfaceName);
    std::string thisInterfaceName;
};
/*
 * "Classes" hold specifications
 */
class NexClass{
public:
    NexClass(std::vector<FieldSpec*> *desClassFields,
             std::vector<MethodSpec*> *desClassMethods,
             std::vector<InterfaceSpec*> *desClassInterfaces,
             NexClass * const desSuperClass,
             const std::string &desClassName = std::string("NULL"),
             uint8_t *desConstantPoolBase = NULL);
    NexObjectStaticData* getObjectStaticData();
    MethodSpec* getMethodSpec(uint64_t iter);
    FieldSpec* getFieldSpec(uint64_t iter);
    InterfaceSpec* getInterfaceSpec(uint64_t iter);
    uint64_t getFieldSpecCount();
    uint64_t getMethodSpecCount();
    uint64_t getInterfaceSpecCount();
    uint8_t* getConstantPoolBase();
    ClassID getClassID();
    void setClassID(ClassID newClassID);
    const std::string& getClassName();
    bool verifySuperClass(ClassID superClass);
    bool hasMethodSpecs();
    bool hasFieldSpecs();
    bool hasInterfaceSpecs();
    /*
     * Used to Map Data
     */
protected:
    std::string thisClassName;
    ClassID thisClassID;
    NexObjectStaticData *thisClassObjectStaticData;
    NexClass * superClass;
    std::vector<FieldSpec*> *classFields; //Overriding PTR
    std::vector<MethodSpec*> *classMethods; //Overriding PTR
    std::vector<InterfaceSpec*> *classInterfaces;
    uint8_t *constantPoolBase;
    /*
     * These fields are static
     */
};
/*
 * Object Definitions
 */
class NexObjectStaticData{
public:
    NexObjectStaticData(NexClass *newClass);
    uint64_t getFieldDataCount();
    FieldDataBlock* getFieldData(FieldID fieldID);
    // Reference Stuff
    FieldID getFieldReference(const std::string &fieldName);
    MethodID getMethodReference(const std::string &methodName);
    // Data Acquisition Stuff
    NexMethod* getMethod(uint64_t reference);
    NexInterface* getInterface(uint64_t reference);
    // Misc Stuff
    uint64_t getFieldsSize();
    ClassID getThisClassID();
    bool verifySuperClass(ClassID superClass);
    //DLT
    uint8_t* getClassConstantPoolBase();
private:
    uint64_t fieldCount, fieldStorageSize;
    std::unordered_map<std::string, FieldID> *objectFields;
    std::unordered_map<std::string, MethodID> *objectMethods;
    std::unordered_map<std::string, InterfaceID> *objectInterfaces;
    std::vector<NexMethod*> *objectInnerMethods;
    std::vector<NexInterface*> *objectInnerInterfaces;
    std::vector<FieldDataBlock> *objectFieldAttributes;
    NexClass *thisClass;
};

class NexObject
{
public:
    NexObject(NexObjectStaticData *staticObjectData);
    // Reference Functions (Get/Mod)
    // Field Functions
    uint64_t getField(bool verifyFlag, ClassID callingClass, FieldID reference);
    uint64_t getField(bool verifyFlag, ClassID callingClass, FieldID reference, uint64_t arrayOffset);
    void modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint8_t value);
    void modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint16_t value);
    void modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint32_t value);
    void modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t value);
    void modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t arrayOffset, uint8_t value);
    void modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t arrayOffset, uint16_t value);
    void modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t arrayOffset, uint32_t value);
    void modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t arrayOffset, uint64_t value);
    // Method Functions
    NexObjectStaticData* getStaticData();
    // Misc Stuff
    void checkField(FieldID dataOffset, enum FieldType thisFieldType, uint64_t arrayOffset);
    void checkField(uint64_t dataOffset, enum FieldType thisFieldType);
    // Protection Bits Stuff
    void checkObjectPermissions(ClassID callingClass, AccessType thisAccessType);
private:
    // Object fields are contigousily mapped
    NexField *objectInnerFields;
    // Methods *arent*
    NexObjectStaticData *thisStaticData; //Maps fields to string names via hashtable
};
/*
 * NexObjectDispatch LTM
 */

class NexObjectDispatch{
public:
    NexObjectDispatch();
    ObjectID registerNexObject(NexClass *initalizer_class);
    NexObject* getObject(ObjectID id);
    // Method Invocation Stuff..
    ClassID getCurClassID();
    ObjectID getCurObjectID();
    void PushExecutionFrame(NexExecutionFrame *pushFrame);
    NexExecutionFrame* PopExecutionFrame();
private:
    ObjectID curObjectID;
    ClassID curClassID;
    std::vector<NexObject*> *loadedObjectList;
    std::vector<NexExecutionFrame*> *methodInvocationStack;
    std::unordered_map<std::string, NexClass*> *loadedClassList;
};

}


#endif // NEXOBJECT_H
