#ifndef CLASSLOADER_H
#define CLASSLOADER_H

#include <stdint.h>
#include <vector>

#include <NexVM/object/NexObject.h>

namespace nexvm{

//const uint64_t classMagicNumber = 0xDEC0DE00BABB1E0D;
const uint64_t classMagicNumber = 0xd1ebbba00dec0de;
const uint64_t classMinVersion = 00;

struct RawClassHeader{
    uint64_t classMagic;
    uint64_t minVersion;
    uint64_t headerSize;
    uint64_t constantTableSize;
    uint64_t methodAreaSize;
    uint64_t superClassConstantEntry;
    uint64_t fieldEntries;
    uint64_t methodEntries;
    uint64_t interfaceEntries;
};
struct RawClassFieldEntry{
    enum FieldType fieldType;
    uint64_t fieldSize;
    uint64_t fieldNameOffsetCtable;
    AccessType fieldAccessType : 8;
    FieldType fieldAccessSize : 8;
    uint64_t unused : 48;
};
struct RawClassMethodEntry{
    uint64_t methodOffset;
    uint64_t methodSize;
    uint64_t methodNameOffsetCtable;
    AccessType fieldAccessType : 4;
    FieldType fieldAccessSize : 4;
    uint64_t unused : 56;
};
struct RawClassInterfaceEntry{
    uint64_t interfaceNameOffsetCtable;
};

class RawClassWrapper{
public:
    RawClassWrapper(RawClassHeader *destClassHeader,  uint8_t *methodAreaBase,
                    std::vector<RawClassFieldEntry*> *destClassFields,
                    std::vector<RawClassMethodEntry*> *destClassMethods,
                    std::vector<RawClassInterfaceEntry*> *destClassInterfaces,
                    uint8_t *destConstantTable, uint64_t destConstantTableSize);
    std::vector<FieldSpec*>* getFieldSpecs();
    std::vector<MethodSpec*>* getMethodSpecs();
    std::vector<InterfaceSpec*>* getInterfaceSpecs();
    const std::string& getSuperClassName();
    uint8_t* getConstantTableBase();
private:
    const std::string& getConstantTableString(uint64_t offset);
    uint64_t getConstantTableInt(uint64_t offset);
    RawClassHeader *thisClassHeader;
    std::vector<RawClassFieldEntry*> *thisClassFields;
    std::vector<RawClassMethodEntry*> *thisClassMethods;
    std::vector<RawClassInterfaceEntry*> *thisClassInterfaces;
    std::vector<FieldSpec*>* thisClassFieldSpecs;
    std::vector<MethodSpec*>* thisClassMethodSpecs;
    std::vector<InterfaceSpec*>* thisClassInterfaceSpecs;
    uint8_t *thisConstantTable;
    uint8_t *thisMethodArea;
    uint64_t thisConstantTableSize;
};

class ClassLoader
{
public:
    ClassLoader(std::vector<std::string> *desClassLoadingSites);
    NexClass *getClass(const std::string &classname);
private:
    NexClass *loadClass(const std::string &classname);
    RawClassWrapper* loadClassWrapper(const std::string &classname);
    std::vector<std::string> *thisClassLoadingSites;
    std::unordered_map<std::string, NexClass*> *loadedClasses;
    const std::string& getConstantTableString(uint64_t offset);
    uint64_t getConstantTableInt(uint64_t offset);
    const std::string& fetchConstantString(uint8_t *constantPoolbase, uint64_t constantPoolSize, uint64_t offset);
    uint64_t fetchConstantInt(uint8_t *constantPoolbase, uint64_t constantPoolSize, uint64_t offset);
};

}
#endif // CLASSLOADER_H
