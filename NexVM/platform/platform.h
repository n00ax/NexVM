#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>

namespace nexvm{

class MappingDescriptor{
public:
    uint64_t size;
    uint8_t *base;
};

class Platform{
public:
    static MappingDescriptor* mapFile(const std::string &fileName);
    void unMapFile(void* file); //Unimplemented
};

}

#endif // PLATFORM_H
