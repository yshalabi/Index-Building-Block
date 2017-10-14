
#include "binary-util.h"

/*
 * Make() - Get a sequence of certain size. If there is any prior data, they
 *          will be freed
 * 
 * The return storage is guaranteed to be zeroed out
 */
void BitSequence::Make(size_t new_size) {
  // If size is 0, just err and exit
  always_assert(new_size != 0UL);
  if(data_p != nullptr) {
    delete[] data_p;
  }

  size_t new_byte_size = (new_size + 7) / 8;

  data_p = new uint8_t[new_byte_size];
  memset(data_p, 0x00, new_byte_size);

  // It is the bit length
  length = new_size;

  return;
}

/*
 * SetBit() - This function sets a given bit in the bit sequence
 * 
 * If the index is not valid, the assertion would fail. The return
 * value indicates the previous state of the bit
 */
bool BitSequence::SetBit(size_t pos, bool value) {
  always_assert(pos < length);

  size_t byte_offset = BYTE_OFFSET(pos);
  size_t bit_offset = BIT_OFFSET(pos);

  uint8_t mask = static_cast<uint8_t>(0x01) << bit_offset;
  // If the bit is 1 this will be true
  bool ret = !!(data_p[byte_offset] & mask);

  if(value == false) {
    data_p[byte_offset] &= (~mask);
  } else {
    data_p[byte_offset] |= mask;
  }

  return ret;
}

/*
 * GetBit() - Returns a bit on given position
 */
bool BitSequence::GetBit(size_t pos) const {
  always_assert(pos < length);
  return !!(data_p[BYTE_OFFSET(pos)] & 
            (static_cast<uint8_t>(0x01) << BIT_OFFSET(pos))
           );
}