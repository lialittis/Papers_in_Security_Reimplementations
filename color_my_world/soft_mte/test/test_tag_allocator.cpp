#include "tag_allocator.h"
#include <cassert>
#include <iostream>
#include <ctime>

void test_allocation() {
    void* tagged_ptr = tagged_malloc(64);
    assert(tagged_ptr != NULL);
    std::cout << "[TEST] Allocation passed ✅\n";
    tagged_free(tagged_ptr);
}

void test_tag_integrity() {
    void* tagged_ptr = tagged_malloc(32);
    assert(validate_pointer(tagged_ptr));
    std::cout << "[TEST] Tag validation passed ✅\n";
    tagged_free(tagged_ptr);
}

void test_tag_mismatch() {
    void* tagged_ptr = tagged_malloc(16);
    uint8_t fake_tag = (get_pointer_tag(tagged_ptr) + 1) & 0xF; // Create a different tag
    uintptr_t raw_ptr = (uintptr_t)tagged_ptr & PTR_MASK;
    void* forged_ptr = (void*)(raw_ptr | ((uintptr_t)fake_tag << 60)); // Fake tagged pointer

    assert(!validate_pointer(forged_ptr));
    std::cout << "[TEST] Tag mismatch detection passed ✅\n";
    tagged_free(tagged_ptr);
}

int main() {
    srand(time(NULL)); // Random seed
    test_allocation();
    test_tag_integrity();
    test_tag_mismatch();
    std::cout << "All tests passed! 🎉\n";
    return 0;
}
