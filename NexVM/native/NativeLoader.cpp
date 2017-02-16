#include "NativeLoader.h"

NativeMethod::NativeMethod(const std::string &methodName){

}
void NativeMethod::invokeMethod(){

}

NativeLoader::NativeLoader(){
    thisNativeLibraries = new std::vector<NativeLibrary*>();
}
NativeLibraryReference NativeLoader::getLibraryReference(const std::string &libraryName){
    return thisNativeLibraryReferences->at(libraryName);
}

