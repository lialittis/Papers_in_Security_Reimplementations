# Understanding the Shadow Tagged Base & Bounds (Shadow TBB) Algorithm

The Shadow Tagged Base & Bounds (Shadow TBB) algorithm is a novel memory safety checking mechanism designed 
to detect spatial and temporal memory violations in GPU applications with minimal performance overhead. 
This document outlines the core concepts and components of the Shadow TBB algorithm to provide a foundational 
understanding for its implementation.

## 1. Core Concepts

### 1.1 Base and Size Table (BST)
The BST is a disjoint structure used to store metadata for each memory allocation. Each entry in the BST 
consists of:
- **Base Address:** Starting address of the memory allocation.
- **Size:** Total size of the allocated memory.
- **Tag:** A random non-zero value used to detect temporal memory safety violations.

### 1.2 Pointer Tagging
Pointers are augmented with additional bits (from the upper unused bits) to store either:
- A **BST-entry index** if the number of allocations is within a certain limit.
- A **4-bit random tag** and use of a shadow map when allocations exceed this limit.

### 1.3 Shadow Map
A two-level page-table-like structure that maps pointer values to their corresponding BST-entry index. This 
ensures scalable metadata retrieval when the number of allocations exceeds the direct tagging capacity.

## 2. Lifecycle of a Memory Allocation

### 2.1 Allocation Creation
- On a memory allocation (e.g., `cudaMalloc`), an entry is added to the BST.
- A random non-zero tag is assigned.
- The pointer is tagged with the BST index or mapped in the shadow map.

### 2.2 Pointer Load
- When a pointer is loaded, the BST-entry index or shadow map is used to retrieve metadata.
- Metadata is propagated with the pointer to perform safety checks.

### 2.3 Memory Access Check
Before any memory operation, a check is performed to ensure:
- **Spatial Safety:** Access is within the allocated bounds.
- **Temporal Safety:** Tags in the pointer and BST match.

### 2.4 Allocation Deletion
- On `cudaFree`, the BST entry's tag is invalidated (set to zero).
- Future accesses using stale pointers trigger safety violations.

## 3. Advantages of Shadow TBB
- **High Error Detection Coverage:** Tracks exact bounds, catching both adjacent and non-adjacent overflows.
- **Low Performance Overhead:** Efficient metadata retrieval via pointer tagging and shadow maps.
- **Scalability:** Handles large numbers of allocations without excessive memory overhead.

## 4. Implementation Overview
- **Instrumentation:** Compiler pass adds checks and metadata fetching logic.
- **Runtime Support:** CUDA driver manages BST and shadow map updates.
- **Optimizations:** Loop invariant checks and coalesced bounds checks reduce redundant operations.

This understanding of the Shadow TBB algorithm forms the basis for its incremental re-implementation, starting with constructing the BST and basic memory tracking mechanisms.

