#include "File.h"

File::File(std::string pathname, int flags) {
  fd_ = open(pathname.c_str(), flags);
  struct stat st {};
  fstat(fd_, &st);
  size_ = st.st_size;
  mem_start_ = (char*)mmap(NULL, size_, PROT_WRITE, MAP_SHARED, fd_, 0);
  lockf(fd_, F_LOCK, 0);
}

ssize_t File::read(void* buf, size_t size) {
  ssize_t stored = 0;
  for (int i = 0; i < size && position_ < size_; i++, position_++, stored++) {
    reinterpret_cast<char*>(buf)[position_] = mem_start_[i];
  }
  return stored;
}

ssize_t File::write(void* buf, size_t size) {
  ssize_t saved = 0;
  for (int i = 0; i < size && position_ < size_; i++, position_++, saved++) {
    mem_start_[i] = reinterpret_cast<char*>(buf)[position_];
  }
  return saved;
}

File::~File() {
  lockf(fd_, F_ULOCK, 0);
  munmap(mem_start_, size_);
  close(fd_);
}