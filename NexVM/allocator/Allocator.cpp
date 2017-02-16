#include "Allocator.h"
#include "NexVM/exception/ExceptionHandeler.h"

//POSIX Specific
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nexvm{
Allocator::Allocator(){

}
void* Allocator::getAllocation(uint64_t size){

}
void Allocator::allocateClass(std::string& class_name){
    int32_t open_file_descriptor = open(class_name.append(".nexo").c_str(), O_RDONLY);
    struct stat file_attributes;
    fstat(open_file_descriptor, &file_attributes);
    uint64_t class_file_size = static_cast<uint64_t>(file_attributes.st_size);
    if(open_file_descriptor == -1){
        nexvm::ExceptionHandeler::ExceptionHandeler(VM_MEMORY_ALLOCATION_ERR);
    }

}

}
