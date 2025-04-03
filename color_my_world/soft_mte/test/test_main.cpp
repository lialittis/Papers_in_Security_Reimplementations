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

    std::cout << "[PASS]✅ memcpy with valid tags worked!" << std::endl;

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

    std::cout << "[PASS]✅ memcpy with mismatched tags should have failed!" << std::endl;

    tagged_free(src);
    tagged_free(dest);
}

void test_pointer_addition_preserves_tag() { // TODO: is there any need for tag preseving operations?
    void* ptr = tagged_malloc(64);
    uint8_t original_tag = get_pointer_tag(ptr);

    void* new_ptr = (void*)((uintptr_t)ptr + 16);
    uint8_t new_tag = get_pointer_tag(new_ptr);

    if (original_tag != new_tag) {
        std::cout << "❌ Tag mismatch after pointer arithmetic!" << std::endl;
        tagged_free(ptr);  // Free memory
        exit(EXIT_FAILURE);
    } else {
        std::cout << "[PASS]✅ test_pointer_addition_preserves_tag passed!" << std::endl;
        tagged_free(ptr);  // Free memory
    }
}

void test_pointer_subtraction_preserves_tag() { // TODO: is there any need for tag preseving operations?
    void* ptr = tagged_malloc(64);
    uint8_t original_tag = get_pointer_tag(ptr);

    void* new_ptr = (void*)((uintptr_t)ptr - 16);
    uint8_t new_tag = get_pointer_tag(new_ptr);

    if (original_tag != new_tag) {
        std::cout << "❌ Tag mismatch after pointer arithmetic!" << std::endl;
        tagged_free(ptr);  // Free memory
        exit(EXIT_FAILURE);
    } else {
        std::cout << "[PASS]✅ test_pointer_addition_preserves_tag passed!" << std::endl;
        tagged_free(ptr);  // Free memory
    }
}

void test_array_indexing_preserves_tag() {
    int* array = (int*)tagged_malloc(10 * sizeof(int));
    uint8_t original_tag = get_pointer_tag(array);

    for(int i = 0; i < 10; ++i) {
        int* element = &array[i];

        uint8_t new_tag = get_pointer_tag(element);
        if (original_tag != new_tag) {
            std::cout << "❌ Tag mismatch after pointer arithmetic!" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "[DEBUG] Dereferencing "<< array[i] << std::endl;
    }
    std::cout << "[PASS]✅ test_array_indexing_preserves_tag passed!" << std::endl;
}

void test_array_dereferencing() {
    int* array = (int*)tagged_malloc(10 * sizeof(int));

    for(int i = 0; i < 10; ++i) {
        array[i] = i * 10;
        std::cout << "[DEBUG] Dereferencing "<< array[i] << std::endl;
    }
    std::cout << "[PASS]✅ test_array_deferencing passed!" << std::endl;
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

    test_pointer_addition_preserves_tag();
    test_pointer_subtraction_preserves_tag();
    test_array_indexing_preserves_tag();
    return 0;
}
