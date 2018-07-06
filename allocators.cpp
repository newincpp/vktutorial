#include <iostream>
#include <stdlib.h>
#include "allocators.h"

// eastl allocator
void* operator new[](unsigned long size, char const* pname, int flags, unsigned int debugflags, char const* file,  int line) {
    // TODO: handle debug draw and memory consuption
    // TODO: CPU/GPU shared memory and syncronisation using `int flags` ?
    #ifdef DEBUG
    if (pname && file && line) {
	    std::cout << "unaligned allocation: " << pname << " " << file << " " << line << '\n';
    } 
    #endif
    return malloc(size);
}

        
#ifdef __APPLE__ // on OSX C11 doesn't seems to be supported
void* aligned_alloc(size_t alignment_, size_t size_) {
    void* p;
    int error;
    
    error = posix_memalign(&p, alignment_, size_);
    return (error == 0 ? p : NULL);
}
#endif
        
// eastl aligned allocator
void* operator new[](unsigned long size, unsigned long alignment, unsigned long alignmentOffset, char const* pname, int flags, unsigned int debugFlags, char const* file, int line) {
#ifdef DEBUG
    if (pname && file && line) {
	    std::cout << "aligned allocation: " << pname << " " << file << " " << line << '\n';
    }
#endif
    return aligned_alloc(alignment, size);
}

//TODO vulkan allocator compatible with eastl allocator
