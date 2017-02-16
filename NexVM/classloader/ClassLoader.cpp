#include "ClassLoader.h"
#include "NexVM/exception/ExceptionHandeler.h"
#include "NexVM/platform/platform.h"
#include "NexVM/exception/Logger.h"
#include <stdio.h>

namespace nexvm{
/*
 * [RawClassWrapper]
 * Wraps Class Interfaces, and Provides Specification Templtates
 */
RawClassWrapper::RawClassWrapper(RawClassHeader *destClassHeader, uint8_t *methodAreaBase,
                                 std::vector<RawClassFieldEntry*> *destClassFields,
                                 std::vector<RawClassMethodEntry*> *destClassMethods,
                                 std::vector<RawClassInterfaceEntry*> *destClassInterfaces,
                                 uint8_t *destConstantTable, uint64_t destConstantTableSize){
    this->thisClassFields = destClassFields;
    this->thisClassMethods = destClassMethods;
    this->thisClassInterfaces = destClassInterfaces;
    this->thisClassHeader = destClassHeader;
    this->thisClassFieldSpecs = new std::vector<FieldSpec*>();
    this->thisClassMethodSpecs = new std::vector<MethodSpec*>();
    this->thisClassInterfaceSpecs = new std::vector<InterfaceSpec*>();
    this->thisConstantTable = destConstantTable;
    this->thisConstantTableSize = destConstantTableSize;
    this->thisMethodArea = destConstantTable;
    if(thisClassFields != NULL){
        for(uint64_t class_copy_field_ctr=0;class_copy_field_ctr<thisClassFields->size();class_copy_field_ctr++){
            thisClassFieldSpecs->push_back(new nexvm::FieldSpec(
                                         getConstantTableString(thisClassFields->at(class_copy_field_ctr)->
                                          fieldNameOffsetCtable), thisClassFields->at(class_copy_field_ctr)->fieldType,
                                          thisClassFields->at(class_copy_field_ctr)->fieldSize,
                                               thisClassFields->at(class_copy_field_ctr)->fieldAccessType));
         }
    }
    if(thisClassMethods != NULL){
        for(uint64_t class_copy_method_ctr=0;class_copy_method_ctr<thisClassMethods->size();class_copy_method_ctr++){
            thisClassMethodSpecs->push_back(new nexvm::MethodSpec(
                                            getConstantTableString(thisClassMethods->at(class_copy_method_ctr)->
                                            methodNameOffsetCtable),reinterpret_cast<uint8_t*>(thisMethodArea+
                                                (thisClassMethods->at(class_copy_method_ctr)->methodOffset)),
                                                thisClassMethods->at(class_copy_method_ctr)->methodSize,
                                                thisClassMethods->at(class_copy_method_ctr)->fieldAccessType));
        }
    }
    if(thisClassInterfaces != NULL){
        for(uint64_t class_copy_interface_ctr=0;class_copy_interface_ctr<thisClassInterfaces->size();
            class_copy_interface_ctr++){
                thisClassInterfaceSpecs->push_back(new nexvm::InterfaceSpec(
                                           getConstantTableString(thisClassInterfaces->at(class_copy_interface_ctr)->
                                                                     interfaceNameOffsetCtable)));
        }
    }
}
std::vector<FieldSpec*> *RawClassWrapper::getFieldSpecs(){
    return thisClassFieldSpecs;
}
std::vector<MethodSpec*> *RawClassWrapper::getMethodSpecs(){
    return thisClassMethodSpecs;
}
std::vector<InterfaceSpec*> *RawClassWrapper::getInterfaceSpecs(){
     return thisClassInterfaceSpecs;
}
const std::string& RawClassWrapper::getConstantTableString(uint64_t offset){
    if(offset >= thisConstantTableSize){
        nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_CONSTANT_OFFSET);
        return 0;
    }
    else{
         return *(new std::string(
                     reinterpret_cast<const char*>(
                         reinterpret_cast<nexvm::constantPoolEntryString*>(thisConstantTable+offset)->string),
                      reinterpret_cast<nexvm::constantPoolEntryString*>(thisConstantTable+offset)->string_size));
    }

}
const std::string& RawClassWrapper::getSuperClassName(){
    return getConstantTableString(thisClassHeader->superClassConstantEntry);
}

uint64_t RawClassWrapper::getConstantTableInt(uint64_t offset){
    if(offset > thisConstantTableSize){
        nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_INVALID_CONSTANT_OFFSET);
        return 0; //Make Clang Quiet
    }
    else{
        return *(thisConstantTable + offset);
    }
}
uint8_t* RawClassWrapper::getConstantTableBase(){
    return thisConstantTable;
}

/*
 * [ClassLoader]
 * Loads the Actual Class Descriptors into Memory and Maps Sections
 * Offsets are indeterminent, and Endianness is Little Endian
 */
ClassLoader::ClassLoader(std::vector<std::string> *desClassLoadingSites){
    loadedClasses = new std::unordered_map<std::string, NexClass*>();
    thisClassLoadingSites = desClassLoadingSites;
    for(uint64_t class_loading_ctr=0;class_loading_ctr<desClassLoadingSites->size();class_loading_ctr++){

    }
}
NexClass* ClassLoader::getClass(const std::string &classname){
    if(classname == "<NoSuper>"){
        return NULL;
    }
    if(loadedClasses->find(classname) == loadedClasses->end()){
        return loadClass(classname);
    }
    else{
        return loadedClasses->at(classname);
    }
}
NexClass* ClassLoader::loadClass(const std::string &classname){
    RawClassWrapper *new_class_wrapper = loadClassWrapper(classname);
    return new NexClass(new_class_wrapper->getFieldSpecs(),
                        new_class_wrapper->getMethodSpecs(),
                        new_class_wrapper->getInterfaceSpecs(),
                        getClass(new_class_wrapper->getSuperClassName()),
                        classname, new_class_wrapper->getConstantTableBase());
}
RawClassWrapper* ClassLoader::loadClassWrapper(const std::string &classname){
    nexvm::MappingDescriptor *raw_map;
    for(uint64_t class_path_ctr = 0; class_path_ctr < thisClassLoadingSites->size(); class_path_ctr++){
        raw_map = Platform::mapFile(thisClassLoadingSites->at(class_path_ctr)+classname);
        if(raw_map->base == reinterpret_cast<void*>(-1) && class_path_ctr == thisClassLoadingSites->size()-1){
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_CLASS_NOT_FOUND, classname);
        }
        if(raw_map->base != reinterpret_cast<void*>(-1)){
            break;
        }
    }
    RawClassHeader *this_class_header = reinterpret_cast<RawClassHeader*>(raw_map->base);
    if(sizeof(RawClassHeader)>raw_map->size){
       // nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_MALFORMED_HEADER_FILE_TOO_SMALL);
    }
    if(this_class_header->classMagic != classMagicNumber /*|| this_class_header->minVersion != classMinVersion*/){
        nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_HEADER_MAGIC_OR_SIZE_INVALID);
    }
    /*
     * Load Specs
     */
    NEX_LOG((std::string("Loading Class : [")+std::string(classname)+std::string("]")).c_str());
    //List Class Data...
    NEX_LOG((std::string("Total Field Entries : [")+std::to_string(this_class_header->fieldEntries)+
             std::string("]")).c_str());
    NEX_LOG((std::string("Total Method Entries : [")+std::to_string(this_class_header->methodEntries)+
             std::string("]")).c_str());
    NEX_LOG((std::string("Total Interface Entries : [")+std::to_string(this_class_header->interfaceEntries)+
             std::string("]")).c_str());
    RawClassFieldEntry *this_class_field_base = reinterpret_cast<RawClassFieldEntry*>((raw_map->base)+sizeof(RawClassHeader));
    std::vector<RawClassFieldEntry*> *mapped_field_specs = new std::vector<RawClassFieldEntry*>();
    for(uint64_t field_counter=0;field_counter<this_class_header->fieldEntries;field_counter++){
        mapped_field_specs->push_back(this_class_field_base);
        this_class_field_base++;
        if(reinterpret_cast<uint8_t*>(this_class_field_base) >reinterpret_cast<uint8_t*>(this_class_header+raw_map->size)){
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_FIELDS_OVERFLOW);
        }
    }
    RawClassMethodEntry *this_class_method_base = reinterpret_cast<RawClassMethodEntry*>(this_class_field_base);
    std::vector<RawClassMethodEntry*> *mapped_method_specs = new std::vector<RawClassMethodEntry*>();
    for(uint64_t method_counter=0;method_counter<this_class_header->methodEntries;method_counter++){
        mapped_method_specs->push_back(this_class_method_base);
        this_class_method_base++;
        if(reinterpret_cast<uint8_t*>(this_class_method_base)>reinterpret_cast<uint8_t*>(this_class_header+raw_map->size)){
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_FIELDS_OVERFLOW);
        }
    }
    RawClassInterfaceEntry *this_class_interface_base = reinterpret_cast<RawClassInterfaceEntry*>(this_class_method_base);
    std::vector<RawClassInterfaceEntry*> *mapped_interface_specs = new std::vector<RawClassInterfaceEntry*>();
    for(uint64_t field_counter=0;field_counter<this_class_header->interfaceEntries;field_counter++){
        mapped_interface_specs->push_back(this_class_interface_base);
        this_class_interface_base++;
        if(reinterpret_cast<uint8_t*>(this_class_interface_base) >reinterpret_cast<uint8_t*>(this_class_header+raw_map->size)){
            nexvm::ExceptionHandeler::handleInternalState(nexvm::VM_FIELDS_OVERFLOW);
        }
    }
    //Next Time To Map Method Area
    uint8_t *constant_table_base = reinterpret_cast<uint8_t*>(this_class_interface_base)-sizeof(this_class_interface_base)+8;
    uint8_t *method_area_base = constant_table_base;
    return new RawClassWrapper(this_class_header, method_area_base,
                               mapped_field_specs,
                               mapped_method_specs,
                               mapped_interface_specs,
                               constant_table_base, this_class_header->constantTableSize);
}
}
