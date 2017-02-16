#include <NexVM/platform/Platform.h>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <iostream>
namespace nexvm{

MappingDescriptor* Platform::mapFile(const std::string &fileName){
    int32_t this_file_descriptor = open(fileName.c_str(), O_RDONLY);
    struct stat stat_buffer;
    fstat(this_file_descriptor, &stat_buffer);
    MappingDescriptor *this_mapping = new MappingDescriptor();
    this_mapping->base = reinterpret_cast<uint8_t*>(mmap(0, static_cast<uint64_t>(stat_buffer.st_size),
                                                         PROT_READ, MAP_SHARED, this_file_descriptor, 0));
    this_mapping->size = static_cast<uint64_t>(stat_buffer.st_size);
    return this_mapping;
}

}

#endif
