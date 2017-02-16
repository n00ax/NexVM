#ifndef NATIVELOADER_H
#define NATIVELOADER_H

#include <vector>
#include <unordered_map>

#include <NexVM/object/NexObject.h>

typedef uint64_t NativeMethodID, NativeLibraryID;
typedef void* NativeMethodReference;
typedef void* NativeLibraryReference;

struct NativeArgumentPassingObject{
    NativeArgumentPassingObject(uint64_t d);
};

class NativeMethod{
public:
    NativeMethod(const std::string &methodName);
    void invokeMethod();
    void *methodHandle;
};

class NativeLibrary{
public:
    NativeLibrary(const std::string &libraryName);
    NativeMethod* getMethod(NativeMethodID callingMethodID);
private:
    std::vector<NativeMethod*> *thisNativeMethods;
};

class NativeLoader{
public:
    NativeLoader();
    NativeLibraryReference getLibraryReference(const std::string &libraryName);
    NativeMethod* getLibrary(NativeLibraryID callingLibraryID);
private:
    std::vector<NativeLibrary*> *thisNativeLibraries;
    std::unordered_map<std::string, NativeLibraryReference> *thisNativeLibraryReferences;
};

#endif // NATIVELOADER_H
