#include "tag_allocator.h"
#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unordered_map>

static std::unordered_map<uintptr_t, uint8_t> shadow_memory;

uint8_t generate_tag() {
  return rand() & 0xF;  // Random tag between 0x0 and 0xF
}

void* tagged_malloc(size_t size) {
    std::cout << "[DEBUG] Entering tagged_malloc: Requested size = " << size << std::endl;
    std::cout.flush();  // Ensure immediate output
    if (size == 0) return nullptr;  // Handle zero-size allocations

    uint8_t tag = generate_tag();  // Pseudo-random tag
    void* ptr = malloc(size);
    if (!ptr) return nullptr;  // Ensure malloc succeeded

    uintptr_t raw_addr = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t tagged_ptr = (raw_addr & PTR_MASK) | (static_cast<uintptr_t>(tag) << 60);

    std::cout << "[DEBUG] Before shadow_memory assignment, raw_addr = " << raw_addr << std::endl;
    std::cout << "[DEBUG] shadow_memory size = " << shadow_memory.size() << std::endl;
    std::cout.flush();
    shadow_memory.insert({raw_addr, tag});
    shadow_memory[raw_addr] = tag;  // Efficient insertion
    std::cout << "[DEBUG] tagged_malloc: Allocated " << size
              << " bytes at " << raw_addr
              << " with tag " << (int)tag
              << " -> Tagged pointer: " << tagged_ptr << std::endl;
    std::cout.flush();
    return reinterpret_cast<void*>(tagged_ptr);
}

void tagged_free(void* tagged_addr) {
  uintptr_t raw_addr = (uintptr_t)tagged_addr & PTR_MASK;
  if (shadow_memory.find(raw_addr) != shadow_memory.end()) {
    shadow_memory.erase(raw_addr);
  }
  free(reinterpret_cast<void*>(raw_addr));
}

uint8_t get_pointer_tag (void* tagged_ptr) {
  return ((uintptr_t)tagged_ptr & TAG_MASK) >> 60;
}

uint8_t get_shadow_tag(void* tagged_ptr) {
  uintptr_t raw_pointer = (uintptr_t)tagged_ptr & PTR_MASK;
  auto it = shadow_memory.find(raw_pointer);

  if (it != shadow_memory.end()) {
    std::cout << "[DEBUG] Found tag: " << (int)it->second << std::endl;
    return it->second;
  } else {
    return 0x00;
  }
}

int validate_pointer(void* tagged_ptr) {
  uint8_t actual_tag = get_pointer_tag(tagged_ptr);
  uint8_t expected_tag = get_shadow_tag(tagged_ptr);
  return actual_tag == expected_tag; 
}

void set_shadow_tag (uintptr_t raw_pointer, uint8_t tag) {
  shadow_memory[raw_pointer] = tag;
  std::cout << "[DEBUG] set_shadow_tag: Addr = " << raw_pointer << ", Tag = " << (int)tag << std::endl;
  std::cout.flush();
}
