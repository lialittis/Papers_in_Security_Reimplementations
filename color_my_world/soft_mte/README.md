# Softare-based MTE

## **Step 1: Implement a Basic Tagged Memory Allocator**

Before diving into LLVM modifications and runtime enforcement, we need a **custom memory allocator** that assigns **tags to pointers** and stores them in a shadow memory.

### **🔹 Goals for This Step**

- Implement **tagged allocations** (store a tag in the pointer).
- Implement **shadow memory** to store tags separately.
- Implement **basic validation** to check tags before access.
- Write **unit tests** to verify correct tagging behavior.

---

### **1️⃣ Define the Tagging Mechanism**

Since x86 lacks MTE hardware, we need to **simulate tagged pointers** using the upper bits of a pointer.
- Use the **upper 4 bits** of the pointer for a tag (`0xF000000000000000` mask).
- Store the actual pointer **without the tag** (`0x0FFFFFFFFFFFFFFF` mask).

---

### **2️⃣ Implement a Basic Tagged `malloc/free`**
We will create `tagged_malloc` and `tagged_free`, which:  
- Generate a **random tag** for each allocation.
- Store the **tag in shadow memory**.
- Return a **tagged pointer**.

---

### **3️⃣ Explanation**
1. **Tagging Strategy**:
   - Stores a **random tag** in the upper 4 bits of the pointer.
   - Keeps the **actual pointer address** unchanged.

2. **Shadow Memory**:
   - Maps **raw memory addresses** to **their assigned tag**.
   - Used to **validate pointer accesses**.

3. **Validation Function (`validate_pointer`)**:
   - Extracts the **tag from the pointer**.
   - Compares it with the **expected tag in shadow memory**.

4. **Freeing Memory (`tagged_free`)**:
   - Strips the **tagged bits**.
   - Removes the entry from **shadow memory**.

## **Step 2: Implement Runtime Tag Validation for Memory Accesses**

Now that we have a **basic tagged allocator**, we need to enforce **runtime tag validation** for memory accesses. Since x86 lacks hardware MTE, we’ll simulate **tag mismatch detection** in software.

---

### **📌 Goals for This Step**
1. **Intercept memory accesses** (loads & stores).
2. **Validate pointer tags before usage**.
3. **Trigger a simulated fault on mismatched tags**.
4. **Use `LD_PRELOAD` to apply these checks dynamically**.


We’ll use the **ptrace mechanism** and **LD_PRELOAD-based hooking** to:
- Monitor memory reads/writes.
- Check if the pointer’s tag matches the **expected tag in shadow memory**.
- **Abort execution** (or log an error) if a mismatch is found.

---

### **1️⃣ Hooking `memcpy` and `memset` with `LD_PRELOAD`**

Most real-world memory accesses occur via functions like `memcpy` and `memset`.
We will **wrap these functions**, extract **pointer tags**, and validate them before access.

#### **Dynamic Function Hooking (`dlsym`)**
```cpp
static memcpy_t real_memcpy = (memcpy_t)dlsym(RTLD_NEXT, "memcpy");
```
- We use `dlsym(RTLD_NEXT, "memcpy")` to find the **original `memcpy` implementation**.
- `RTLD_NEXT` tells `dlsym` to **look beyond our shared library**, ensuring we call the real `memcpy`, not our own hook.

#### **Wrapping Hooks in a C++ Class**
```cpp
class TagChecker {
public:
    static void *hook_memcpy(void *dest, const void *src, size_t n) {
        static memcpy_t real_memcpy = (memcpy_t)dlsym(RTLD_NEXT, "memcpy");

        if (!validate_pointer((void*)src) || !validate_pointer(dest)) {
            std::cerr << "[MTE ERROR] Memory access violation! Tag mismatch detected." << std::endl;
            std::abort();
        }
        return real_memcpy(dest, src, n);
    }
};
```
- This makes the **hooking logic reusable and modular**.
- `static` methods ensure we don't need to instantiate `TagChecker`.

---

#### **Exposing Functions for `LD_PRELOAD`**
```cpp
extern "C" {
    void *memcpy(void *dest, const void *src, size_t n) {
        return TagChecker::hook_memcpy(dest, src, n);
    }
}
```
- `extern "C"` **prevents C++ name mangling**, making sure our functions match what `LD_PRELOAD` expects.
- This lets us **override** `memcpy` globally in a running program.



### **2️⃣ Validating Memory Access**
```cpp
if (!validate_pointer((void*)src) || !validate_pointer(dest)) {
    std::cerr << "[MTE ERROR] Memory access violation! Tag mismatch detected." << std::endl;
    std::abort();
}
```
- The function `validate_pointer(ptr)` ensures:
  - The memory has **a valid tag**.
  - The pointer **matches expected tags** in shadow memory.
- If validation fails, it prints an **error message** and **terminates execution**.

---

## **🚀 Step 3: Implement Tag Propagation for Pointer Arithmetic**

Now that **tagged memory allocation** and **runtime validation** are in place, the next step is ensuring **tag propagation during pointer arithmetic**.

---

### **🔍 What Needs to Be Implemented?**
1. **Ensure pointer tags persist when performing arithmetic** (e.g., `ptr + offset`).
2. **Prevent invalid tag modifications** (e.g., `ptr | 0xF000000000000000` should not introduce forged tags).
3. **Modify load/store operations** to maintain tags across calculations.
4. **Support pointer manipulation functions** such as `memcpy`, `memmove`, and `realloc`.

---

### **🔑 Key Components**
| Component            | Purpose |
|----------------------|---------|
| **Tagged Arithmetic** | Preserve tags when doing pointer arithmetic. |
| **Tag-Checking Wrapper** | Ensure that after pointer arithmetic, the resulting pointer is still valid. |
| **Safe Load/Store** | Update shadow memory for newly computed addresses. |
| **Tag Forgery Prevention** | Disallow setting arbitrary tag bits manually. |

---

#### **1️⃣ Modify Pointer Arithmetic Functions**

We redefine arithmetic operations to preserve pointer tags.

- **Tagged Pointer Addition**
- **Tagged Pointer Subtraction**
---

#### **2️⃣ Wrap Load/Store Operations**

When loading or storing data, ensure the pointer is still tagged correctly.

- **Safe Memory Load**
- **Safe Memory Store**

---

### **3️⃣ Prevent Tag Forgery**
Ensure that manual modifications of tags are **not allowed**.

---

## **🛠️ Next Steps**
1. ✅ **Implement these functions** into your project.
2. ✅ **Update test cases** to validate pointer arithmetic behavior.
3. ✅ **Extend `memcpy` and `memmove` hooks** to handle tagged pointers correctly.
4. ✅ **Integrate with runtime validation** for enhanced security.
