#ifndef TAG_ALLOCATOR_H
#define TAG_ALLOCATOR_H

#include <cstdint>
#include <cstdlib>

#define TAG_MASK 0xF000000000000000
#define PTR_MASK 0x0FFFFFFFFFFFFFFF

/* extern std::unordered_map<uintptr_t, uint8_t> shadow_memory; */

void* tagged_malloc(size_t size);
void tagged_free(void* tagged_ptr);
uint8_t get_pointer_tag(void* tagged_ptr);
uint8_t get_shadow_tag(void* tagged_ptr);
int validate_pointer(void* tagged_ptr);

#ifdef TESTING
#pragma message("TESTING macro is defined");
  void set_shadow_tag (uintptr_t raw_pointer, uint8_t tag);
#endif // TESTING

#endif // TAG_ALLOCATOR_H
