#include <cesil/util.h>
#include <iostream>

int charToInt(char c){
  return static_cast<int>(c);
}

bool remainsNewLine(std::istream& in){
  char c = in.peek();
  while(c == ' ' || c == '\t'){
    in >> std::noskipws >> c;
    c = in.peek();
  }
  //  std::cout << "\nChar peeked in remainsNewLine: " << c << ' ' << charToInt(c);
  if(in.peek() == '\n'){
    return true;
  }
  else{
    return false;
  }
}

bool isDigit(char c){
  if(c >= '0' && c <= '9'){
    return true;
  }
  else{
    return false;
  }
}
