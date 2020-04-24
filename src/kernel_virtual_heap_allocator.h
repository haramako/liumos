#pragma once

#include "generic.h"
#include "paging.h"

class KernelVirtualHeapAllocator {
 public:
  KernelVirtualHeapAllocator(IA_PML4& pml4,
                             PhysicalPageAllocator& dram_allocator)
      : next_base_(kKernelHeapBaseAddr),
        pml4_(pml4),
        dram_allocator_(dram_allocator){};
  template <typename T>
  T AllocPages(uint64_t num_of_pages, uint64_t page_attr) {
    return MapPages<T>(dram_allocator_.AllocPages<uint64_t>(num_of_pages),
                       num_of_pages, page_attr);
  }
  template <typename T>
  T MapPages(uint64_t paddr, uint64_t num_of_pages, uint64_t page_attr) {
    uint64_t byte_size = (num_of_pages << kPageSizeExponent);
    if (byte_size > kKernelHeapSize ||
        next_base_ + byte_size > kKernelHeapBaseAddr + kKernelHeapSize)
      Panic("Cannot allocate kernel virtual heap");
    uint64_t vaddr = next_base_;
    next_base_ += byte_size + (1 << kPageSizeExponent);
    CreatePageMapping(dram_allocator_, pml4_, vaddr, paddr, byte_size,
                      page_attr);
    return reinterpret_cast<T>(vaddr);
  }

  template <typename T>
  T* Alloc() {
    return AllocPages<T*>(ByteSizeToPageSize(sizeof(T)),
                          kPageAttrPresent | kPageAttrWritable);
  }

 private:
  static constexpr uint64_t kKernelHeapBaseAddr = 0xFFFFFFFF90000000;
  static constexpr uint64_t kKernelHeapSize = 0x0000000040000000;
  uint64_t next_base_;
  IA_PML4& pml4_;
  PhysicalPageAllocator& dram_allocator_;
};
