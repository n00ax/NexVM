#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdint.h>
#include <vector>
#include <string>
#include "NexVM/object/ClassDefinitions.h"

namespace nexvm{

class Allocator{
public:
    Allocator();
    void allocateClass(std::string& class_name);
    void* getAllocation(uint64_t size);
private:

};

}
#endif // ALLOCATOR_H
