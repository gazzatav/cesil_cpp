#ifndef MNEMONIC_H_
#define MNEMONIC_H_

#include <cstdint>
#include <string>
// Add NOOP to enable parse of '%', '*' and line with just label.
enum class Mnemonic:uint8_t{
  UNKNOWN, NOOP, PRINT, LINE, IN, OUT, LOAD, STORE, ADD,
  SUBTRACT, MULTIPLY, DIVIDE, JUMP, JINEG, JIZERO, HALT
};


std::string mnemonicToString(Mnemonic nem);
Mnemonic strToMnemonic(std::string str);

#endif
