// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_UV_BUFFER_H_
#define WPINET_UV_BUFFER_H_

#include <uv.h>

#include <cstring>
#include <initializer_list>
#include <string_view>
#include <utility>

#include <wpi/SmallVector.h>
#include <wpi/span.h>

namespace wpi::uv {

/**
 * Data buffer.  Convenience wrapper around uv_buf_t.
 */
class Buffer : public uv_buf_t {
 public:
  Buffer() {
    base = nullptr;
    len = 0;
  }
  /*implicit*/ Buffer(const uv_buf_t& oth) {  // NOLINT
    base = oth.base;
    len = oth.len;
  }
  /*implicit*/ Buffer(std::string_view str)  // NOLINT
      : Buffer{str.data(), str.size()} {}
  /*implicit*/ Buffer(span<const uint8_t> arr)  // NOLINT
      : Buffer{reinterpret_cast<const char*>(arr.data()), arr.size()} {}
  Buffer(char* base_, size_t len_) {
    base = base_;
    len = static_cast<decltype(len)>(len_);
  }
  Buffer(const char* base_, size_t len_) {
    base = const_cast<char*>(base_);
    len = static_cast<decltype(len)>(len_);
  }

  span<const char> data() const { return {base, len}; }
  span<char> data() { return {base, len}; }

  operator span<const char>() const { return data(); }  // NOLINT
  operator span<char>() { return data(); }              // NOLINT

  static Buffer Allocate(size_t size) { return Buffer{new char[size], size}; }

  static Buffer Dup(std::string_view in) {
    Buffer buf = Allocate(in.size());
    std::memcpy(buf.base, in.data(), in.size());
    return buf;
  }

  static Buffer Dup(span<const uint8_t> in) {
    Buffer buf = Allocate(in.size());
    std::memcpy(buf.base, in.begin(), in.size());
    return buf;
  }

  Buffer Dup() const {
    Buffer buf = Allocate(len);
    std::memcpy(buf.base, base, len);
    return buf;
  }

  void Deallocate() {
    delete[] base;
    base = nullptr;
    len = 0;
  }

  Buffer Move() {
    Buffer buf = *this;
    base = nullptr;
    len = 0;
    return buf;
  }

  friend void swap(Buffer& a, Buffer& b) {
    using std::swap;
    swap(a.base, b.base);
    swap(a.len, b.len);
  }
};

/**
 * A simple pool allocator for Buffers.
 * Buffers are allocated individually but are reused rather than returned
 * to the heap.
 * @tparam DEPTH depth of pool
 */
template <size_t DEPTH = 4>
class SimpleBufferPool {
 public:
  /**
   * Constructor.
   * @param size Size of each buffer to allocate.
   */
  explicit SimpleBufferPool(size_t size = 4096) : m_size{size} {}
  ~SimpleBufferPool() { Clear(); }

  SimpleBufferPool(const SimpleBufferPool& other) = delete;
  SimpleBufferPool& operator=(const SimpleBufferPool& other) = delete;

  /**
   * Allocate a buffer.
   */
  Buffer Allocate() {
    if (m_pool.empty()) {
      return Buffer::Allocate(m_size);
    }
    auto buf = m_pool.back();
    m_pool.pop_back();
    buf.len = m_size;
    return buf;
  }

  /**
   * Allocate a buffer.
   */
  Buffer operator()() { return Allocate(); }

  /**
   * Release allocated buffers back into the pool.
   * This is NOT safe to use with arbitrary buffers unless they were
   * allocated with the same size as the buffer pool allocation size.
   */
  void Release(span<Buffer> bufs) {
    for (auto& buf : bufs) {
      m_pool.emplace_back(buf.Move());
    }
  }

  /**
   * Clear the pool, releasing all buffers.
   */
  void Clear() {
    for (auto& buf : m_pool) {
      buf.Deallocate();
    }
    m_pool.clear();
  }

  /**
   * Get number of buffers left in the pool before a new buffer will be
   * allocated from the heap.
   */
  size_t Remaining() const { return m_pool.size(); }

 private:
  SmallVector<Buffer, DEPTH> m_pool;
  size_t m_size;  // NOLINT
};

}  // namespace wpi::uv

#endif  // WPINET_UV_BUFFER_H_
