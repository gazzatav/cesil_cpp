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
    if(in){
      std::cout << "File is opened.\n";
    }
    else{
      std::cout << "File is not opened.\n";
      return 1;
    }
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
  in.clear();
  in.seekg(0, in.beg);
  while(in.good()){
    prog.emplace_back(parseLine(in));
  }
  if(std::holds_alternative<std::monostate>(prog.back().mnemonicV())){
      prog.pop_back();
    }
  std::cout << "\nOutputting Prog.............\n\n";
  for(Line& l: prog){
    static unsigned i{0};
    std::cout << "At line " << i << '\t' << l << '\n';
    ++i;
  }

  Data data= parseData(in);
  std::cout << '\n';
  for(auto n:data){
    std::cout << "Data entry: " << n << '\t';
  }
  std::cout << '\n';
std::cout << "Debug: program size. " << prog.size() << '\n';
  auto labs = resolveLabels(prog);
  for(auto& add:labs){
    std::cout << add.first << ' ' << add.second << '\n';
  }
  std::map<std::string, int32_t> vars{};
  vars["forty-two"] = 42;
  CesilMachine mac(prog, data, vars);

  //mac.run();
  mac.debug();
}
