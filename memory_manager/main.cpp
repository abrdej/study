#include <iostream>

// Memory manager from book: Optimized C++, Kurt Guntheroth

template <typename Arena>
class fixed_block_memory_manager {
 public:
  template <int N>
  explicit fixed_block_memory_manager(char(&a)[N])
      : free_ptr_(nullptr), block_size_(0), arena_(a) {
  }

  ~fixed_block_memory_manager() = default;
  fixed_block_memory_manager(fixed_block_memory_manager&) = delete;
  fixed_block_memory_manager& operator=(fixed_block_memory_manager&) = delete;

  void* allocate(std::size_t size) {
    if (empty()) {
      free_ptr_ = reinterpret_cast<free_block*>(arena_.allocate(size));
      block_size_ = size;
      if (empty()) {
        throw std::bad_alloc();
      }
    }
    if (size != block_size_) {
      throw std::bad_alloc();
    }
    auto p = free_ptr_;
    free_ptr_ = free_ptr_->next;
    return p;
  }

  std::size_t block_size() const {
    return block_size_;
  }

  std::size_t capacity() const {
    return arena_.capacity();
  }

  void clear() {
    free_ptr_ = nullptr;
    arena_.clear();
  }

  void deallocate(void* p) {
    if (p == nullptr) {
      return;
    }
    auto fp = reinterpret_cast<free_block*>(p);
    fp->next = free_ptr_;
    free_ptr_ = fp;
  }

  bool empty() const {
    return !free_ptr_;
  }

 private:
  struct free_block {
    free_block* next;
  };
  free_block* free_ptr_;
  std::size_t block_size_;
  Arena arena_;
};

class fixed_arena_controller {
 public:
  template <int N>
  explicit fixed_arena_controller(char(&a)[N])
      : arena_(a), arena_size_(N), block_size_(0) {}

  ~fixed_arena_controller() = default;
  fixed_arena_controller(fixed_arena_controller&) = delete;
  fixed_arena_controller& operator=(fixed_arena_controller&) = delete;

  void* allocate(std::size_t size) {
    if (empty()) {
      return nullptr;
    }
    block_size_ = std::max(size, sizeof(void*));
    auto count = capacity();
    if (count == 0) {
      return nullptr;
    }
    char* p;
    for (p = reinterpret_cast<char*>(arena_); count > 1; --count, p += size) {
      *reinterpret_cast<char**>(p) = p + size;
    }
    *reinterpret_cast<char**>(p) = nullptr;
    return arena_;
  }

  std::size_t block_size() const {
    return block_size_;
  }

  std::size_t capacity() const {
    return block_size_ ? (block_size_ / arena_size_) : 0;
  }

  void clear() {
    block_size_ = 0;
  }

  bool empty() const {
    return block_size_ == 0;
  }

 private:
  void* arena_;
  std::size_t arena_size_;
  std::size_t block_size_;
};

class MemoryManagerTester {
 public:
  explicit MemoryManagerTester(int c) : contents_(c) {}

  static void* operator new(size_t s) {
    return manager.allocate(s);
  }
  static void operator delete(void* p) {
    manager.deallocate(p);
  }
  static fixed_block_memory_manager<fixed_arena_controller> manager;

 private:
  int contents_{0};
};

char arena[4004];

fixed_block_memory_manager<fixed_arena_controller> MemoryManagerTester::manager(arena);

int main() {
  MemoryManagerTester a(10);
  MemoryManagerTester b(15);
  return 0;
}
