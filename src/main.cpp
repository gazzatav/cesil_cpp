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
    std::string line;
    while(std::cin.good()){
      std::getline(std::cin, line);
      std::cin >> std::ws;
      in_prog << line << '\n';
      if(std::cin.peek() == '*'){
	break;
      }
    }
  }

  Program lines;
  while(in.good()){
    lines.emplace_back(parseLine(in));
  }
  std::cout << "\nOutputting Lines.............\n\n";
  for(Line& l: lines){
    static unsigned i{1};
    std::cout << "At line " << i << '\t' << l << '\n';
    ++i;
  }
  in.clear();
  in.seekg(0, in.beg);
  Data data= parseData(in);
  std::cout << '\n';
  for(auto n:data){
    std::cout << "Data entry: " << n << '\t';
  }
  std::cout << '\n';

  auto labs = resolveLabels(lines);
  for(auto& add:labs){
    std::cout << add.first << ' ' << add.second << '\n';
  }
  std::map<std::string, int32_t> vars{};
  vars["forty-two"] = 42;
  CesilMachine mac(lines, data, vars);
  mac.run();
}
