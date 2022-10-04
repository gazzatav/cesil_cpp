#include <cesil/cesil.h>
#include <cesil/mnemonic.h>
#include <cesil/version.h>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]){
  std::cout << "Hello from CESIL, this is version "  << version << '\n';
  std::ifstream in;
  if(argc > 1){
    in.open(argv[1]);
  }
  else{
    std::ofstream in_prog("prog_text");
    if(!in_prog){
      return 1;
    }
    std::string line;
    while(std::cin.good()){
      std::getline(std::cin, line);
      std::cin >> std::ws;
      in_prog << line << '\n';
      if(std::cin.peek() == '*'){
	std::getline(std::cin, line);
	in_prog << line;
	in_prog.close();
	break;
      }
    }
    in.open("prog_text");
  }

  Program prog;
  while(in.good()){
    prog.emplace_back(parseLine(in));
  }
  Data data= parseData(in);
  auto labs = resolveLabels(prog);
  std::map<std::string, int32_t> vars{};
  vars["forty-two"] = 42;
  CesilMachine mac(prog, data, vars);
  mac.run();
  //mac.debug();
}
