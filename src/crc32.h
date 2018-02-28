#pragma once 

#include <stddef.h>
#include <stdint.h>

#include <string>

#include <crc32c/crc32c.h>

namespace bordkv {

template <typename Type> 
inline uint32_t crc32(uint32_t crc, const Type *_buf, size_t len) {
    const uint8_t *buf = reinterpret_cast<const uint8_t*>(_buf);
    return crc32c::Extend(crc, buf, len);
}

template <>
inline uint32_t crc32(uint32_t crc, const uint8_t * data, size_t count) {
    return crc32c::Extend(crc, data, count);
}

// Computes the CRC32C of the string's content.
inline uint32_t crc32(uint32_t crc, const std::string& string) {
  return crc32c::Extend(crc, reinterpret_cast<const uint8_t*>(string.data()),
                string.size());
}

#if __cplusplus > 201402L
#if __has_include(<string_view>)
#include <string_view>

// Computes the CRC32C of the bytes in the string_view.
inline uint32_t crc32(uint32_t crc, const std::string_view& string_view) {
  return crc32c::Extend(crc, reinterpret_cast<const uint8_t*>(string_view.data()),
                string_view.size());
}

#endif  // __has_include(<string_view>)
#endif  // __cplusplus > 201402L

} // namespace bordkv
