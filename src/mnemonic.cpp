#include <string>
#include <cesil/mnemonic.h>

std::string mnemonicToString(Mnemonic nem){
  using enum Mnemonic;
  switch(nem){
  case UNKNOWN: return "UNKNOWN";
  case PRINT: return "PRINT";
  case LINE: return "LINE";
  case IN: return "IN";
  case OUT: return "OUT";
  case LOAD: return "LOAD";
  case STORE: return "STORE";
  case ADD: return "ADD";
  case SUBTRACT: return "SUBTRACT";
  case MULTIPLY: return "MULTIPLY";
  case DIVIDE: return "DIVIDE";
  case JUMP: return "JUMP";
  case JINEG: return "JINEG";
  case JIZERO: return "JIZERO";
  case HALT: return "HALT";
  case NOOP: return "NOOP";
  default: return "UNKNOWN";
  }
}

Mnemonic strToMnemonic(std::string str){
  if(str == "PRINT") return Mnemonic::PRINT;
  else if(str == "LINE") return Mnemonic::LINE;
  else if(str == "IN") return Mnemonic::IN;
  else if(str == "OUT") return Mnemonic::OUT;
  else if(str == "LOAD") return Mnemonic::LOAD;
  else if(str == "STORE") return Mnemonic::STORE;
  else if(str == "ADD") return Mnemonic::ADD;
  else if(str == "SUBTRACT") return Mnemonic::SUBTRACT;
  else if(str == "MULTIPLY") return Mnemonic::MULTIPLY;
  else if(str == "DIVIDE") return Mnemonic::DIVIDE;
  else if(str == "JUMP") return Mnemonic::JUMP;
  else if(str == "JINEG") return Mnemonic::JINEG;
  else if(str == "JIZERO") return Mnemonic::JIZERO;
  else if(str == "HALT") return Mnemonic::HALT;
  else return Mnemonic::UNKNOWN;
}
