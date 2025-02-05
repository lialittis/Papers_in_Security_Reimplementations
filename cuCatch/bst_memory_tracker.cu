// Simple Base and Size Table (BST) implementation for memory tracking in CUDA applications
#include <iostream>
#include <unordered_map>
#include <random>
#include <mutex>

// Metadata structure for each allocation
struct Metadata {
    void *base_addr;
    size_t size;
    uint8_t tag; // 4-bit tag for temporal safety
};

class BSTracker {
private:
    std::unordered_map<void*, Metadata> bst;
    std::mutex bst_mutex;
    std::mt19937 rng;
    std::uniform_int_distribution<uint8_t> tag_dist;

public:
    BSTracker() : rng(std::random_device{}()), tag_dist(1, 15) {}

    // Allocate memory and store metadata
    void* allocate(size_t size) {
        void* ptr = nullptr;
        cudaMalloc(&ptr, size);
        if (ptr) {
            Metadata meta = {ptr, size, tag_dist(rng)};
            std::lock_guard<std::mutex> lock(bst_mutex);
            bst[ptr] = meta;
            std::cout << "Allocated " << size << " bytes at " << ptr << " with tag " << (int)meta.tag << "\n";
        }n
        return ptr;
    }


    // Free memory and invalidate metadata
    void deallocate(void* ptr) {
        std::lock_guard<std::mutex> lock(bst_mutex);
        auto it = bst.find(ptr);
        if( it != bst.end() ) {
            it->second.tag = 0; // Invalidate tag
            cudaFree(ptr);
            bst.erase(it);
            std::cout << "Deallocate at " << ptr << "\n";
        } else {
            std::cout << "Attempted to free untracked pointer " << ptr << "\n";
        }
    }

    // Check if access is within bounds
    bool check_bounds(void* ptr, size_t access_size) {
        std::lock_guard<std::mutex> lock(bst_mutex);
        for (const auto& [base, meta] : bst) {
            if (ptr >= base && (char*)ptr + access_size <= (char*)base + meta.size) {
                return true; // Valid access
            }
        }
        return false; // Out-of-bounds access
    }

};


// Example usage
int main() {
    BSTracker tracker;
    void* ptr = tracker.allocate(1024);

    if (tracker.check_bounds(ptr, 512)) {
        std::cout << "Access within bounds." << std::endl;
    } else {
        std::cerr << "Out-of-bounds access detected!" << std::endl;
    }

    tracker.deallocate(ptr);
    return 0;
}
