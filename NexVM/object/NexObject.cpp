#include "NexObject.h"

#include <NexVM/exception/ExceptionHandeler.h>
#include "NexVM/interpreter/BuildConstants.h"

#include <iostream>

namespace nexvm{

/*
 *  [NexMethod]
 *  Stores Pointer to Method Base, cannot be modified
 */
NexMethod::NexMethod(uint8_t *destMethodBytecodeBase, uint64_t destMethodSize, AccessType destMethodAccessType){
    this->methodBytecodeBase = destMethodBytecodeBase;
    this->methodSize = destMethodSize;
    this->methodAccessType = destMethodAccessType;
}
// Should Take Invocation And Push Old Frame, Setting New Frame Pointer
NexExecutionFrame* NexMethod::invokeMethod(uint64_t *stackArgumentBase, uint64_t stackArgumentCount,
                                           uint8_t *newConstantPoolBase, ObjectID thisObjectListId,
                                           ClassID thisObjectClassID, uint64_t oldStackArgumentTop,
                                           ClassID callingClass, NexObjectStaticData *thisCalleeDat){
    /* (SP)(4). Calling Reference ID [Stack] (0). ThisObject Reference [Local Table]
     * (3). Argument Count                   (1). Argument Count
     * (4). Argument 1                       (2). Argument 0
     * (5). Argument 0                       (3). Argument 1
     * (6). ...                              (4). ...
     */
    NexExecutionFrame *newFrame = new NexExecutionFrame();
    newFrame->instructionPointer = 0;
    newFrame->instructionBase = methodBytecodeBase;
    newFrame->localsTableBase = new uint64_t[MAX_LOCALS_SIZE];
    newFrame->localsTableSize = MAX_LOCALS_SIZE;
    newFrame->localStackPointer = new uint64_t[MAX_STACK_SIZE];
    newFrame->localStackSize = MAX_STACK_SIZE;
    newFrame->localsTableBase[0] = thisObjectListId;
    stackArgumentBase -= stackArgumentCount;
    for(uint64_t i=1;i<stackArgumentCount+1;i++){
        newFrame->localsTableBase[i] = *(stackArgumentBase++);
    }
    newFrame->constantPoolBase = newConstantPoolBase;
    return newFrame;
}
void checkMethodPermissions(ClassID thisClass, ClassID callingClass, AccessType thisAccessType){


}

uint64_t NexMethod::getMethodSize(){
    return methodSize;
}
AccessType NexMethod::getMethodAccessType(){
    return methodAccessType;
}

/*
 *  [NexInterface]
 *  Contains Prototype Specifications to expected Fields
 */
NexInterface::NexInterface(InterfaceSpec *interfacePrototype){
    this->thisInterface = interfacePrototype;
}
/*
 *  [FieldSpec]
 *  Specification for Expected Field Characteristics
 */
FieldSpec::FieldSpec(const std::string &fieldName, enum FieldType desFieldType,
                     uint64_t fieldSize, AccessType fieldAccess){
    this->thisFieldName = fieldName;
    this->thisFieldType = desFieldType;
    this->fieldSize = fieldSize;
    this->thisFieldAccessType = fieldAccess;
}
/*
 *  [MethodSpec]
 *  Specification for Extended Method Characteristics
 *  (Contains Real References, not prototypes like FieldSpepc)
 */
MethodSpec::MethodSpec(const std::string &methodName, uint8_t *methodBase, uint64_t methodSize,
                       AccessType methodAccessType){
    this->thisMethodName = methodName;
    this->thisMethodBase = methodBase;
    this->thisMethodSize = methodSize;
    this->thisMethodAccessType = methodAccessType;
}
/*
 *  [InterfaceSpec]
 *  Specification for Expected Interface Characteristics
 *  (Only Contains Expected Interfaces)
 */
InterfaceSpec::InterfaceSpec(const std::string &interfaceName){
    this->thisInterfaceName = interfaceName;
}
/*
 *  [NexClass]
 *  Specification for Objects, Contains All Field Allocation Prototypes
 *  (Only Contains Expected Interfaces)
 */
NexClass::NexClass(std::vector<FieldSpec*> *desClassFields,
                   std::vector<MethodSpec*> *desClassMethods,
                   std::vector<InterfaceSpec*> *desClassInterfaces,
                   NexClass * desSuperClass, const std::string &desClassName,
                   uint8_t *desConstantPoolBase){
    this->classFields = desClassFields;
    this->classMethods = desClassMethods;
    this->classInterfaces = desClassInterfaces;
    // We need to bind the superclasses methods and fields
    this->superClass = desSuperClass;
    this->thisClassObjectStaticData = new NexObjectStaticData(this);
    thisClassName = desClassName;
    this->constantPoolBase = desConstantPoolBase;
    if(desClassName == "<NULL>"){
        nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_METHOD_RESOLUTION_NAME_NULL);
    }
    if(superClass == NULL){ //Base Class Object, if So.....
        return;
    }
    if(superClass->hasFieldSpecs()){
        for(uint64_t field_bind_count=0;field_bind_count<superClass->getFieldSpecCount();field_bind_count++){
            classFields->push_back(desSuperClass->getFieldSpec(field_bind_count));
        }
    }
    if(superClass->hasMethodSpecs()){
        for(uint64_t method_bind_count=0;method_bind_count<superClass->getMethodSpecCount()/*superClass->getMethodSpecCount()*/;method_bind_count++){
            classMethods->push_back(desSuperClass->getMethodSpec(method_bind_count));
        }
    }
    if(superClass->hasInterfaceSpecs()){
        for(uint64_t interface_bind_count=0;interface_bind_count<superClass->getInterfaceSpecCount();interface_bind_count++){
            classInterfaces->push_back(desSuperClass->getInterfaceSpec(interface_bind_count));
        }
    }
}
NexObjectStaticData* NexClass::getObjectStaticData(){
    return thisClassObjectStaticData;
}
FieldSpec* NexClass::getFieldSpec(uint64_t iter){
    return classFields->at(iter);
}
MethodSpec* NexClass::getMethodSpec(uint64_t iter){
    return classMethods->at(iter);
}
InterfaceSpec* NexClass::getInterfaceSpec(uint64_t iter){
    return classInterfaces->at(iter);
}
uint64_t NexClass::getFieldSpecCount(){
    return classFields->size();
}
uint64_t NexClass::getInterfaceSpecCount(){
    return classInterfaces->size();
}
uint64_t NexClass::getMethodSpecCount(){
    return classMethods->size();
}
void NexClass::setClassID(ClassID newClassID){
    thisClassID = newClassID;
}
ClassID NexClass::getClassID(){
    return thisClassID;
}
const std::string& NexClass::getClassName(){
    return thisClassName;
}
bool NexClass::verifySuperClass(ClassID superClass){
    NexClass *currentSuperClass = this;
    while(currentSuperClass != NULL){
        if(currentSuperClass->getClassID() == superClass){
            return true;
        }
        currentSuperClass = currentSuperClass->superClass;
    }
    return false;
}
bool NexClass::hasMethodSpecs(){
    if(classMethods == NULL) return false;
    else return true;
}
bool NexClass::hasFieldSpecs(){
    if(classFields == NULL) return false;
    else return true;
}
bool NexClass::hasInterfaceSpecs(){
    if(classInterfaces == NULL) return false;
    else return true;
}
uint8_t* NexClass::getConstantPoolBase(){
    return constantPoolBase;
}

/*
 *  [NexStaticObjectData]
 *  Contains Class Wide Static Data
 *  Used to Reduce Pointer Overhead For Objects
 */
NexObjectStaticData::NexObjectStaticData(NexClass *newClass){
    this->thisClass = newClass;
    objectFieldAttributes = new std::vector<FieldDataBlock>();
    objectInnerMethods = new std::vector<NexMethod*>();
    objectInnerInterfaces = new std::vector<NexInterface*>();
    objectFields = new std::unordered_map<std::string, FieldID>();
    objectMethods = new std::unordered_map<std::string, MethodID>();
    objectInterfaces  = new std::unordered_map<std::string, InterfaceID>();
    // Method Setup Is A Universal Thing
    if(thisClass->hasMethodSpecs()){
        for(uint64_t method_counter=0;method_counter<thisClass->getMethodSpecCount();method_counter++){
            objectInnerMethods->push_back(new NexMethod(newClass->getMethodSpec(method_counter)->thisMethodBase,
                                                        newClass->getMethodSpec(method_counter)->thisMethodSize,
                                                        newClass->getMethodSpec(method_counter)->thisMethodAccessType));
            objectMethods->emplace(thisClass->getMethodSpec(method_counter)->thisMethodName, method_counter);
        }
    }
    // Interface Setup Also Is....
    if(thisClass->hasInterfaceSpecs()){
        for(uint64_t interface_counter=0;interface_counter<thisClass->getInterfaceSpecCount();interface_counter++){
            objectInnerInterfaces->push_back(new NexInterface(newClass->getInterfaceSpec(interface_counter)));
            objectInterfaces->emplace(newClass->getInterfaceSpec(interface_counter)->thisInterfaceName, interface_counter);
        }
    }
    // We Must Setup The Field Attributes, But Not Allocate
    if(thisClass->hasFieldSpecs()){
        for(uint64_t field_mapping_counter=0;field_mapping_counter < thisClass->getFieldSpecCount(); field_mapping_counter++){
            FieldDataBlock newDataBlock;
            newDataBlock.fieldAccessSize = thisClass->getFieldSpec(field_mapping_counter)->thisFieldType;
            newDataBlock.fieldAccessType = thisClass->getFieldSpec(field_mapping_counter)->thisFieldAccessType;
            newDataBlock.arraySize = thisClass->getFieldSpec(field_mapping_counter)->fieldSize;
            newDataBlock.isSynchronized = thisClass->getFieldSpec(field_mapping_counter)->isSynchronized;
            objectFieldAttributes->push_back(newDataBlock);
            objectFields->emplace(thisClass->getFieldSpec(field_mapping_counter)->thisFieldName, field_mapping_counter);
        }
        for(uint64_t byte_counter = 0; byte_counter < getFieldDataCount(); byte_counter++){
            fieldStorageSize += getFieldData(byte_counter)->fieldAccessType;
            objectFieldAttributes->at(byte_counter).fieldOffset = fieldStorageSize;
        }
    }
}
uint64_t NexObjectStaticData::getFieldDataCount(){
    return objectFieldAttributes->size();
}
FieldDataBlock* NexObjectStaticData::getFieldData(FieldID fieldID){
    return &(objectFieldAttributes->at(fieldID));
}
FieldID NexObjectStaticData::getFieldReference(const std::string &fieldName){
    return objectFields->at(fieldName);
}
MethodID NexObjectStaticData::getMethodReference(const std::string &methodName){
    return objectMethods->at(methodName);
}
NexMethod* NexObjectStaticData::getMethod(uint64_t reference){
    return objectInnerMethods->at(reference);
}
NexInterface* NexObjectStaticData::getInterface(uint64_t reference){
    return objectInnerInterfaces->at(reference);
}
uint64_t NexObjectStaticData::getFieldsSize(){
    return fieldStorageSize;
}
ClassID NexObjectStaticData::getThisClassID(){
    return thisClass->getClassID();
}
bool NexObjectStaticData::verifySuperClass(ClassID superClass){
    return thisClass->verifySuperClass(superClass);
}
uint8_t* NexObjectStaticData::getClassConstantPoolBase(){
    return thisClass->getConstantPoolBase();
}

/*
 *  [NexObject]
 *  Contains the Actual Object Representation
 *  Binded Methods are Duplicated Within the Symbol Table, Although The Older
 *  Method is the original signature
 */
NexObject::NexObject(NexObjectStaticData *classPrototype){

    this->thisStaticData = classPrototype;
    this->objectInnerFields = new uint8_t[thisStaticData->getFieldsSize()];
}
uint64_t NexObject::getField(bool verifyFlag, ClassID callingClass, FieldID desField){
    if(!verifyFlag){
        checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);
    }
    switch(thisStaticData->getFieldData(desField)->fieldAccessSize){
        case BYTE:
            if(!verifyFlag) checkField(desField, BYTE);
            return *(reinterpret_cast<uint8_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset));
        case WORD:
            if(!verifyFlag) checkField(desField, WORD);
            return *(reinterpret_cast<uint16_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset));
        case DWORD:
            if(!verifyFlag) checkField(desField, DWORD);
            return *(reinterpret_cast<uint32_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset));
        case QWORD:
            if(!verifyFlag) checkField(desField, QWORD);
            return *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset));
        case FIELD_ARRAY_BYTES:
        case FIELD_ARRAY_WORDS:
        case FIELD_ARRAY_DWORDS:
        case FIELD_ARRAY_QWORDS:
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_FIELD_ARR_NO_SIZE);
        default:
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_FIELD_SIZE);
   }
   return 0;
}
uint64_t NexObject::getField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t arrayOffset){
    if(!verifyFlag){
        checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);
    }
    switch(thisStaticData->getFieldData(desField)->fieldAccessSize){
        case FIELD_ARRAY_BYTES:
            if(!verifyFlag) checkField(desField, BYTE, arrayOffset);
            return *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset
                                                 )+arrayOffset);
        case FIELD_ARRAY_WORDS:
            if(!verifyFlag) checkField(desField, WORD, arrayOffset);
            return *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset
                                                )+arrayOffset);
        case FIELD_ARRAY_DWORDS:
            if(!verifyFlag) checkField(desField, DWORD, arrayOffset);
            return *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset
                                            )+arrayOffset);
        case FIELD_ARRAY_QWORDS:
            if(!verifyFlag) checkField(desField, QWORD, arrayOffset);
            return *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset
                                            )+arrayOffset);
        case BYTE:
        case WORD:
        case DWORD:
        case QWORD:
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_FIELD_NONARR_SIZE);
        default:
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_FIELD_SIZE);

    }
    return 0;
}

void NexObject::modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint8_t value){
    //Get object descriptor
    if(!verifyFlag){
        checkField(desField, BYTE);
        checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);
    }
    *(reinterpret_cast<uint8_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset)) =
            value;
}
void NexObject::modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint16_t value){
    if(!verifyFlag){
        checkField(desField, WORD);
        checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);
    }
    *(reinterpret_cast<uint16_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset)) =
            value;
}
void NexObject::modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint32_t value){
    if(!verifyFlag){
            checkField(desField, DWORD);
            checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);

    }
    *(reinterpret_cast<uint32_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset)) =
            value;
}
void NexObject::modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t value){
    if(!verifyFlag){
        checkField(desField, QWORD);
        checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);
    }
    *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset)) =
            value;
}
void NexObject::modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t arrayOffset, uint8_t value){
    if(!verifyFlag){
        checkField(desField, FIELD_ARRAY_BYTES, arrayOffset);
        checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);
    }
    *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset +
                                  thisStaticData->getFieldData(desField)->fieldOffset)) = value;
}
void NexObject::modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t arrayOffset, uint16_t value){
    if(!verifyFlag){
        checkField(desField, FIELD_ARRAY_WORDS, arrayOffset);
        checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);
    }
    *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset +
                                  thisStaticData->getFieldData(desField)->fieldOffset)) =  value;
}
void NexObject::modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t arrayOffset, uint32_t value){
    if(!verifyFlag){
        checkField(desField, FIELD_ARRAY_DWORDS, arrayOffset);
        checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);
    }
    *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset +
                                  thisStaticData->getFieldData(desField)->fieldOffset)) = value;
}
void NexObject::modField(bool verifyFlag, ClassID callingClass, FieldID desField, uint64_t arrayOffset, uint64_t value){
    if(!verifyFlag){
        checkField(desField, FIELD_ARRAY_QWORDS, arrayOffset);
        checkObjectPermissions(callingClass, thisStaticData->getFieldData(desField)->fieldAccessType);
    }
    *(reinterpret_cast<uint64_t*>(objectInnerFields + thisStaticData->getFieldData(desField)->fieldOffset +
                                  thisStaticData->getFieldData(desField)->fieldOffset)) = value;
}
NexObjectStaticData* NexObject::getStaticData(){
    return thisStaticData;
}
void NexObject::checkField(FieldID dataOffset, enum FieldType thisFieldType , uint64_t arrayOffset){
    try{
        if(thisStaticData->getFieldData(dataOffset)->arraySize <= arrayOffset){
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_ARR_FIELD_OUT_OF_BOUNDS);
        }
    }
    catch(std::exception){
        nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_FIELD_DOES_NOT_EXIST, std::to_string(dataOffset));
    }
    checkField(dataOffset, thisFieldType);
}
void NexObject::checkField(FieldID dataOffset, enum FieldType thisFieldType){
    try{
        if(thisFieldType != thisStaticData->getFieldData(dataOffset)->fieldAccessSize){
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_FIELD_SIZE);
        }
        if(thisStaticData->getFieldDataCount() < dataOffset){
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_FIELD_WRITE_OFFSET);
        }
    }
    catch(std::exception){
        nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_FIELD_DOES_NOT_EXIST, std::to_string(dataOffset));
    }
}
void NexObject::checkObjectPermissions(ClassID callingClass, AccessType thisAccessType){
    if(thisAccessType == IS_PRIVATE && callingClass != getStaticData()->getThisClassID()){
        nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_PRIVATE_ACCESS_VIOLATION);
    }
    if(thisAccessType == IS_PROTECTED && getStaticData()->verifySuperClass(callingClass)){
        nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_PROTECTED_ACCESS_VIOLATION);
    }
    if(thisAccessType == IS_PUBLIC){
        return;
    }
}

/*
 *  [NexObjectDispatch]
 *  Contains All Objects Binded Within The Virtual Machine Reference Space
 *  A Relative Reference May Be Loaded, Which Allows Auto Specification
 */
NexObjectDispatch::NexObjectDispatch(){
    loadedClassList = new std::unordered_map<std::string, NexClass*>();
    loadedObjectList = new std::vector<NexObject*>();
    methodInvocationStack = new std::vector<NexExecutionFrame*>();
}
ObjectID NexObjectDispatch::registerNexObject(NexClass *initalizerClass){
    initalizerClass->setClassID(loadedObjectList->size()+1);
    loadedObjectList->push_back(new NexObject(initalizerClass->getObjectStaticData()));
    return loadedObjectList->size()-1;
}
NexObject* NexObjectDispatch::getObject(ObjectID id){
    try{
        return loadedObjectList->at(id);
    }
    catch(std::exception){
        nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_OBJECT_DOES_NOT_EXIST, std::to_string(id));
    }
}
ClassID NexObjectDispatch::getCurClassID(){
    return curClassID;
}
ObjectID NexObjectDispatch::getCurObjectID(){
    return curObjectID;
}

void NexObjectDispatch::PushExecutionFrame(NexExecutionFrame *pushFrame){
    methodInvocationStack->push_back(pushFrame);
}
NexExecutionFrame* NexObjectDispatch::PopExecutionFrame(){
    NexExecutionFrame *retPtr = methodInvocationStack->back();
    methodInvocationStack->pop_back();
    curObjectID = retPtr->thisFrameObjectID;
    curClassID = retPtr->thisFrameClassID;
    return retPtr;
}
}

