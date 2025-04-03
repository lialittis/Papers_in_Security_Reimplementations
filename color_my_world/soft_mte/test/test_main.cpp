#include <cstdint>
#include <iostream>
#include <cstdlib>
#include "tag_allocator.h"
#include "tag_checker.h"

void set_memory_tag(void**, uint8_t);

void test_valid_memcpy() {
    std::cout << "[TEST] Valid memcpy with tagged memory..." << std::endl;

    void* src = tagged_malloc(64);
    void* dest = tagged_malloc(64);

    if (!src || !dest) {
        std::cerr << "[ERROR] Memory allocation failed!" << std::endl;
        return;
    }

    //Simulate setting a valid tag
    set_memory_tag(&src, 0b1100);
    set_memory_tag(&dest, 0b1100);

    // Perform memcpy (should succeed)
    memcpy(dest, src, 64);

    std::cout << "[PASS] memcpy with valid tags worked!" << std::endl;

    tagged_free(src);
    // tagged_free(dest);
}

void test_invalid_memcpy() {
    std::cout << "[TEST] Invalid memcpy with mismatched tags..." << std::endl;

    void* src = tagged_malloc(64);
    void* dest = tagged_malloc(64);

    if (!src || !dest) {
        std::cerr << "[ERROR] Memory allocation failed!" << std::endl;
        return;
    }

    // Set different memory tags
    set_memory_tag(&src, 0b1100);
    set_memory_tag(&dest, 0b1010); // Different tag!

    // Perform memcpy (should trigger MTE error)
    memcpy(dest, src, 64);

    std::cout << "[FAIL] memcpy with mismatched tags should have failed!" << std::endl;

    tagged_free(src);
    tagged_free(dest);
}

void set_memory_tag(void** ptr, uint8_t tag) {
  uintptr_t custom_tag = (uintptr_t)tag << 60;
  uintptr_t raw_addr = (uintptr_t)(*ptr) & PTR_MASK;
  *ptr = (void*)(raw_addr | custom_tag);
#ifdef TESTING
  set_shadow_tag(raw_addr, tag);  // Should compile with TESTING macro
  std::cout << "[DEBUG] set_memory_tag: Updated " << raw_addr
  << " with tag " << (int)tag << std::endl;
  std::cout.flush();
#endif //TESTING
}

int main() {
  std::cout << "Running MTE Memory Safety Tests...\n" << std::endl;

  test_valid_memcpy();
  test_invalid_memcpy();

  return 0;
}
