# Instructions

To re-implement methods from the *cuCatch* paper, I suggest breaking down the project into smaller, manageable components. Here’s a step-by-step plan focusing on simpler parts first:

### 1. **Understand the Shadow TBB Algorithm**
   - **Goal:** Grasp how cuCatch uses the Shadow Tagged Base & Bounds (Shadow TBB) algorithm for memory safety.
   - **Action:** Implement a simplified version of the Base and Size Table (BST) to store metadata (base address, size, tag) for memory allocations.

### 2. **Implement Basic Memory Tracking**
   - **Goal:** Track CUDA memory allocations (`cudaMalloc`) and deallocations (`cudaFree`).
   - **Action:** Create a wrapper around these functions to update the BST.

### 3. **Add Metadata Fetching**
   - **Goal:** Simulate the compiler's insertion of metadata fetching instructions.
   - **Action:** For each pointer load, fetch metadata from the BST to verify memory access.

### 4. **Develop Basic Bounds Checking**
   - **Goal:** Implement spatial memory safety.
   - **Action:** Add checks before memory access to ensure pointers do not exceed their allocated bounds.

### 5. **Implement Temporal Safety (Tag Checks)**
   - **Goal:** Catch use-after-free errors.
   - **Action:** Add tagging logic where each allocation gets a random tag and validate this tag during memory access.

### 6. **Optimize with Loop Invariant Checks**
   - **Goal:** Reduce performance overhead.
   - **Action:** Apply optimizations like hoisting loop-invariant bounds checks out of loops.

### 7. **Progress to Compiler Instrumentation**
   - **Goal:** Automate code instrumentation.
   - **Action:** Use LLVM to insert safety checks into PTX code.

Starting with memory tracking and simple safety checks builds a foundation for the more complex instrumentation and optimizations described in the paper.
