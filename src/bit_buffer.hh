#pragma once

#include <bitset>

template <int BufferSize>
struct BitBuffer {
  std::bitset<BufferSize> bitBuffer;
  bool getAndEnable(std::size_t n) {
    const bool ret = bitBuffer[BufferSize - 1];
    bitBuffer <<= 1;
    if (BufferSize <= 64) {
      uint64_t mask = (1 << n) - 1;
      bitBuffer |= mask;
    } else {
      for (int i = 0; i < n; i++) {
        bitBuffer.set(i, true);
      }
    }
    return ret;
  }
};
