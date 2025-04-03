#ifndef TAG_CHECKER_H
#define TAG_CHECKER_H

#include <cstddef>  // For size_t

class TagChecker {
public:
    static void* hook_memcpy(void* dest, const void* src, size_t n);
    static void* hook_memset(void* ptr, int value, size_t num);
};

extern "C" {
    void* memcpy(void* dest, const void* src, size_t n);
    void* memset(void* ptr, int value, size_t num);
}

#endif // TAG_CHECKER_H
