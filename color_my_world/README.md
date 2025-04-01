# Color My World: Deterministic Tagging for Memory Safety

To re-implement or partially implement the memory safety approach mentioned in the source, we would need to prepare several aspects of our development environment and gain a thorough understanding of the described system. Here's a breakdown of the key areas:

*   **Hardware Support for ARMv8.5 Memory Tagging Extension (MTE)**: The described approach heavily relies on the ARMv8.5 instruction set architecture (ISA) feature, the Memory Tagging Extension (MTE). Therefore, we would need access to **hardware that supports ARMv8.5-A MTE**. The paper mentions that processors with MTE are being deployed. Alternatively, we could use an **emulator that replicates MTE functionality**, such as QEMU 6.0.0. The emulator allows we to test the compatibility between our instrumentation and MTE.

*   **LLVM Compiler Framework (Version 12 or later)**: The implementation in the paper is based on the **LLVM compiler framework**, specifically version 12. We would need to **set up an environment with LLVM Clang** (the C/C++ frontend for LLVM) and the necessary development tools to build and modify the compiler. Familiarity with the LLVM Intermediate Representation (IR) would be crucial as the analysis and instrumentation operate at this level.

*   **Understanding the Static Memory Safety Analysis**: The core of the approach involves a **static analysis pass** that classifies memory allocations based on their safety properties. This analysis extends the existing LLVM StackSafetyAnalysis. We would need to **understand the principles of static analysis**, particularly data-flow analysis, and how it is implemented within LLVM. Specifically, we should study how the analysis tracks pointers and their usage to determine if an allocation is memory-safe, guardable, or unsafe. The concepts of base pointers and the UseInfo container object are important.

*   **Implementing Compiler Instrumentation Passes**: The paper describes two new LLVM IR passes: **MTStack** and **MTStackLoad**.
    *   **MTStack** is responsible for **tagging both pointers and memory allocations** based on the results of the memory safety analysis. This includes inserting llvm.aarch64.settag intrinsics for unsafe allocations and managing tag resets on function return. It also handles padding and alignment of tagged allocations to the 16-byte MTE granule size.
    *   **MTStackLoad** implements **tag forgery prevention** by clearing the topmost tag bit of loaded pointers (unless loaded from a pointer-safe allocation) and preventing pointer arithmetic from modifying address tags. We would need to learn how to create new LLVM passes that can traverse and modify the IR by inserting new instructions (intrinsics).

*   **Modifications to the LLVM Backend (AArch64)**: To generate machine code that utilises the MTE instructions, modifications to the **AArch64 backend of LLVM** are necessary. This includes lowering the compiler intrinsics (like llvm.aarch64.settag and llvm.aarch64.memoryguard) to the appropriate MTE machine instructions. Understanding the process of instruction selection and code generation in the LLVM backend would be beneficial.

*   **Handling Guarded Allocations**: The approach uses memory guards to prevent linear overflows for allocations that are not provably safe but only allow linear overflows. We would need to understand how the static analysis identifies such "guarded" allocations and how the **MTStack pass inserts memory guard allocations** using the llvm.aarch64.memoryguard intrinsic. The **AArch64MemoryGuard pass** in the backend is responsible for optimising and potentially removing redundant memory guards.

*   **Tagging Strategy**: We need to understand the **tagging strategy** employed, where unsafe allocations and pointers based on them have the topmost tag bit cleared (e.g., 0b0xxx), safe allocations use a default tag (e.g., 0b1100), and pointer-unsafe but otherwise safe allocations use another tag (e.g., 0b10xx). We would need to implement this logic in our instrumentation passes.

*   **Runtime Environment Setup**: The paper mentions modifying the **libc runtime library** to enable MTE at the start of the process. We might need to investigate how to configure and enable MTE within our target operating system and potentially modify the startup code of our programs. The paper also notes that the default tag on mainline Linux can be set.

*   **Testing and Evaluation Framework**: To verify our implementation, we would need a **testing framework**. The paper used the SPEC CPU 2017 benchmarks for evaluation. We would need a similar set of test programs to assess the security, functionality, and performance of our implementation. Using an MTE-enabled emulator like QEMU is essential for functional verification. For performance evaluation, the paper used an MTE overhead analogue on hardware without MTE support.

## Task 0. Simulate MTE-like Behavior on x86

I want to implement a **software-based tagging system**.

### a. Define a Memory Tagging Scheme

Since x86 does not support hardware memory tagging, we could build a softare-based shadow memory approach:

- Each memory allocation gets a *random tag* (like MTE).
- Pointers using that allocation are also *tagged*.
- Before accessing memory, check if the pointer's tag matches the allocation's tag.

**Tagging Strategy:**

- Allocate extra metadata alongside memory
- Mask and store tags in unused bits of pointers
- Verify tags before every memory access


### b. Build a Basic Tagged Allocator

Let's use the upper 4 bits of the pointer for a tag (`0xF000000000000000`mask), and store
the actual pointer without the tag (`0x0FFFFFFFFFFFFFFF`)

### c. Implement Runtime Tag Validation for Memory Access

Now that we have a basic tagged allocator, then we need to simulate tag mismatch
detection in software, to enforce runtime tag validation for memory accesses.

1. Intercept memory accesses (`load`s & `store`s)
2. Validate pointer tags before usage
3. Trigger a simulated fault on mismatched tags
4. Use `LD_PRELOAD` to apply these check dynamically

**Method:** Hooking `memcpy` and `memset` with `LD_PRELOAD`

#### Key Components

1. Dynamic Function Hooking

    We could use `dlsym(RTLD_NEXT, "memcpy")` to find the original `memcpy` implementation, and `RTLD_NEXT` ensures we call the real `memcpy` by looking beyongd our shared library.

2. Validating Memory Access










