#ifndef CESIL_H_
#define CESIL_H_

#include <cesil/mnemonic.h>
#include <memory>
#include <vector>
#include <variant>
#include <map>
#include <cstdint>
#include <iosfwd>
//#include <iostream>
#include <istream>

using LabelV = std::variant<std::monostate, std::string>;
using MnemonicV = std::variant<std::monostate, Mnemonic>;
using OperandV = std::variant<std::monostate, std::string, int32_t, std::size_t>;

using LabAdds = std::map<std::string, std::size_t>;
using NamedVars = std::map<std::string, int32_t>;
using Literals = std::vector<std::string>;
using Data = std::vector<int32_t>;
class Line;
using Program = std::vector<Line>;

// Line is a pimpl/handle.
class Line{
private:
  class LineImpl;
  std::unique_ptr<LineImpl> line_;
public:
  Line();
  Line(std::variant<std::monostate, std::string>&& l,
       std::variant<std::monostate, Mnemonic>&& m,
       std::variant<std::monostate, std::string, int32_t, std::size_t>&& o);
  ~Line();
  Line(Line const&);
  Line(Line &&)noexcept;
  Line& operator=(Line const&) = delete;
  Line& operator=(Line &&) noexcept;
  LabelV& labelV();
  MnemonicV& mnemonicV();
  OperandV& operandV();

  friend std::ostream& operator<<(std::ostream&, Line&);

};
using Program = std::vector<Line>;
class CesilMachine{
private:
  std::size_t pc;
  std::size_t dc;
  int32_t accumulator;
public:
  CesilMachine(Program prog, Data data, LabAdds labadds);
  void run();
  void debug();
  void step();
  // method for each instruction
};

Line parseLine(std::istream& in);
Data parseData(std::istream& in);
std::ostream& operator<<(std::ostream& out, Line& l);
LabAdds resolveLabels(Program& prog);
#endif
