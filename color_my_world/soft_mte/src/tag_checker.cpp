// #define _GNU_SOURCE
#include <cstddef>
#include <dlfcn.h>
#include <iostream>
#include <sys/cdefs.h>
#include "tag_allocator.h"


using memcpy_t = void *(*)(void *, const void *, size_t);
using memset_t = void *(*)(void *, int, size_t);

class TagChecker {
public:
  static void *hook_memcpy(void *dest, const void *src, size_t n) {
    static memcpy_t real_memcpy = (memcpy_t)dlsym(RTLD_NEXT, "memcpy"); // ensures that real_memcpy is only initialized once per process.
    if (!validate_pointer((void*)src) || !validate_pointer((void*)dest)) {
      std::cerr << "[MTE ERROR] Memory access violation! Tag mismatch detected.\n";
      std::abort();
    }

    void* raw_dest = (void*)((uintptr_t)dest & PTR_MASK);
    void* raw_src = (void*)((uintptr_t)src & PTR_MASK);

    return real_memcpy(raw_dest, raw_src, n);
  }

  static void *hook_memset(void *ptr, int value,size_t num) {
    static memset_t real_memset = (memset_t)dlsym(RTLD_NEXT, "memset");

    if (!real_memset) {
      std::cerr << "[MTE ERROR] Failed to find real memset!" << std::endl;
      std::abort();
    }

    if (!validate_pointer((void*)ptr)) {
      std::cerr << "[MTE ERROR] Memory access violation! Tag mismatch detected.\n";
      std::abort();
    }
    void* raw_ptr = (void*)((uintptr_t)ptr & PTR_MASK);

    return real_memset(raw_ptr, value, num);
  }
};

extern "C"{
  __attribute__((visibility("default")))
  void *memcpy(void *dest, const void *src, size_t n) {
    return TagChecker::hook_memcpy(dest, src, n);
  }

  __attribute__((visibility("default")))
  void *memset(void *ptr, int value, size_t num) {
    return TagChecker::hook_memset(ptr, value, num);
  }
}
