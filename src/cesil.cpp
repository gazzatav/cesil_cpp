#include <cesil/cesil.h>
#include <cesil/mnemonic.h>
#include <cesil/util.h>
#include <cesil/version.h>
#include <iostream>
#include <iomanip>
#include <istream>
#include <fstream>
#include <sstream>
#include <optional>
#include <variant>


struct StartLine{};
struct LabelFound{};
struct MnemonicFound{};
struct OperandFound{};
struct LineDone{};

struct ParseVisitor{
  std::istream& in_;
  std::variant<std::monostate, std::string> col1;
  std::variant<std::monostate, Mnemonic> col2;
  std::variant<std::monostate, std::string, int32_t, std::size_t> col3;

  ParseVisitor(std::istream& in)
    :in_(in){}

  std::variant<StartLine, LabelFound, MnemonicFound, OperandFound, LineDone>
  operator()(StartLine&) {
    std::cout << "\nAt Start:";
    in_ >> std::ws;
    char look_ahead = in_.peek();
    std::cout << "\nLook ahead is: " << look_ahead << ' ' << charToInt(look_ahead);
    // Look for data section
    if(look_ahead == '%'){
      in_.ignore(1000, '\n');
      std::cout << "\nFound data section";
      return StartLine{};
    }
    // Ignore data section and parse separately.
    else if(isDigit(look_ahead)){
      in_.ignore(1000, '\n');
      return StartLine{};
    }

    else if(look_ahead == '('){
      in_.ignore(1000, '\n');
      std::cout <<"\nFound comment.";
      return StartLine{};
    }
    else if(look_ahead == '*'){
      in_.ignore(1000, '\n');
      std::cout << "\nFound end of data section.";
      return StartLine{};
    }
    std::string token;
    in_ >> token;
    if(auto mnemonic = strToMnemonic(token); mnemonic == Mnemonic::UNKNOWN){
      std::cout << "\nFound label: " << token;
      col1 = token;
      return LabelFound{};
    }
    //	    else if(auto m = strToMnemonic(token); token.size() > 0 && m != Mnemonic::UNKNOWN){
    else{
      std::cout << "\nFound Mnemonic: " << mnemonicToString(mnemonic);
      col2 = mnemonic;
      return MnemonicFound{};
    }
  }

  std::variant<StartLine, LabelFound, MnemonicFound, OperandFound, LineDone>
  operator()(LabelFound&) {
    std::cout << "\nIn LabelFound" << std::flush;//flushing for debug
    if(remainsNewLine(in_)){
      in_ >> std::ws;
      col2 = Mnemonic::NOOP;
      return LineDone{};
    }
    std::string token;
    in_ >> token;
    if(auto mnemonic = strToMnemonic(token); mnemonic != Mnemonic::UNKNOWN){
      std::cout << "\tFound Mnemonic: " << token << '\t'
		<< "Mnemonic: " << mnemonicToString(mnemonic);
      col2 = mnemonic;
      return MnemonicFound{};
    }
    else if(token.size() > 0){
      std::cout << "Extra label: " << token << std::endl;
      throw std::runtime_error("Extra label after label (no mnemonic).");
    }
    else{
      throw std::runtime_error("Invalid Program Line in operator()(LabelFound).");
    }
  }

  std::variant<StartLine, LabelFound, MnemonicFound, OperandFound, LineDone>
  operator()(MnemonicFound&) {
    std::cout << "\nIn MnemonicFound";
    // Some mnemonics have no operand.
    if(remainsNewLine(in_)){
      return LineDone{};
    }
    in_ >> std::ws;
    char look_ahead = in_.peek();
    // look for literal
    if(look_ahead == '"'){
      in_.ignore(1);
      std::string literal;
      std::getline(in_, literal, '"');
      std::cout << "\nOperand: " << literal << " is a string literal.";
      col3 = literal;
      return OperandFound{};
    }
    // Look for digits
    else if(isDigit(look_ahead)){
      int digits;
      in_ >> digits;
      std::cout << "\nOperand: " << digits << " is a number.";
      col3 = digits;
      return OperandFound{};
    }
    // Look for jump destination
    else if(look_ahead > 0x20 && look_ahead < 0x7b){
      std::cout << "\nFound jump destination";
      std::string dest;
      in_ >> dest;
      col3 = dest;
      return OperandFound{};
    }

    else{
      throw std::runtime_error("Unrecognised operand type in operator()(MnemonicFound).");
    }
  }

  std::variant<StartLine, LabelFound, MnemonicFound, OperandFound, LineDone>
  operator()(OperandFound&) {
    std::cout << "\nIn OperandFound.";
    while(in_.peek() == ' ' || in_.peek() == '\t'){
      char c;
      in_ >> std::noskipws >> c;
      std::cout << "\nExtracted char is: " << c << ' ' << charToInt(c);
    }
    char p = in_.peek();
    std::cout << "\nPeeked char is: " << p << ' ' << charToInt(p) << '\n';
    if(p == '\n'){
      in_ >> std::ws;
      return LineDone{};
    }
    else{
      throw std::runtime_error("Invalid Program Line in operator()(OperandFound).");
    }
  }

  std::variant<StartLine, LabelFound, MnemonicFound, OperandFound, LineDone>
  operator()(LineDone&) {
    std::cout << "\nLine Done.";
    in_.clear();
    in_ >> std::ws;
    return StartLine{};
  }
};
// Want a visitor that can handle five states:
// EmptyLine, LabelFound, MnemonicFound, OperandFound,
// UnknownError and pseudo state Noop.

// *** PROGRAM REQUIRES MORE DATA *** output when we run out of data.

class Line::LineImpl{
private:
  LabelV label;
  MnemonicV mnemonic;
  OperandV operand;
public:
  LineImpl(LabelV l, MnemonicV m, OperandV o);
  friend class Line;
  friend std::ostream& operator<<(std::ostream&, Line&);
};
Line::LineImpl::LineImpl(LabelV l, MnemonicV m, OperandV o)
  :label(std::move(l)), mnemonic(std::move(m)), operand(std::move(o)){
}
Line::Line(LabelV&& l, MnemonicV&& m, OperandV&& o)
  :line_(std::make_unique<LineImpl>(std::move(l), std::move(m), std::move(o))){
}

Line& Line::operator=(Line&& other) noexcept
{
  std::swap(line_, other.line_);
  return *this;
}
Line::~Line() = default;
Line::Line(Line && rhs)noexcept
  :line_(std::move(rhs.line_)){
}
LabelV& Line::labelV(){
  return line_->label;
}
MnemonicV& Line::mnemonicV(){
  return line_->mnemonic;
}
OperandV& Line::operandV(){
  return line_->operand;
}
Line parseLine(std::istream& in){
  std::variant<StartLine, LabelFound, MnemonicFound, OperandFound, LineDone> parseStatus{};
  auto pv = ParseVisitor{in};
  while(in.good() && !std::holds_alternative<LineDone>(parseStatus)){
    parseStatus = std::visit(pv, parseStatus);
  }
  parseStatus = std::visit(pv, parseStatus);
  return Line{std::move(pv.col1), std::move(pv.col2), std::move(pv.col3)};
}

struct OperandStreamVisitor{
  std::ostream& out_;
  OperandStreamVisitor(std::ostream& out)
    :out_(out){}
  void operator()(std::monostate){
    out_ << std::setw(20) << ' ';
  }
  void operator()(std::string s){
    out_ << std::setw(20) << s;
  }
  void operator()(int32_t i){
    out_ << i;
  }
  void operator()(std::size_t st){
    out_ << st;
  }
  void operator()(Mnemonic m){
    out_ << std::setw(12) << mnemonicToString(m);
  }
};

std::ostream& operator<<(std::ostream& out, Line& l){
  OperandStreamVisitor osv(out);
  std::visit(osv, l.line_->label);
  out << ' ';
  std::visit(osv, l.line_->mnemonic);
  out << ' ';
  std::visit(osv, l.line_->operand);
  return out;
}

Data parseData(std::istream& in){
  std::cout << "\nIn parseData";
  Data data;
  while(in.good()){
    char c = in.peek();
    if(c == '%'){
      in.ignore(1000, '\n');
      while(in.good() && c != '*'){
	std::cout << "\nIn second while loop.";
	int32_t d;
	in >> d;
	in.ignore(1000, '\n');
	c = in.peek();
	std::cout << "\ndatum is " << d;
	data.push_back(d);
      }
    }
    else{
      in.ignore(1000, '\n');
    }
  }
  return data;
}


LabAdds resolveLabels(Program& prog){
  // First pass put labels and address in map.
  // Check they are unique.
  LabAdds labadds;
  for(std::size_t index(0); index < prog.size(); ++index){
    auto alt = prog[index].labelV();
    if(std::holds_alternative<std::string>(alt)){
      std::string s = std::get<std::string>(alt);
      if(labadds.find(s) != labadds.end()){
	std::ostringstream os;
	os << "Error: Lable, " << s << ", used at multiple addresses!";
	throw std::runtime_error(os.str());
      }
      else{
	labadds[s] = index;
      }
    }
  }
  std::cout << "\nFirst pass complete.";
  // Second pass, check all jump instructions and set addresses.
  for(auto& line: prog){
    std::cout << "\nCurrent line: " << line;
    using enum Mnemonic;
    auto mnem = std::get<Mnemonic>(line.mnemonicV()); // every prog line has mnemonic.
    std::cout << "\nBefore mnemonic comparison. mnemonic: " << mnemonicToString(mnem) << " Line " << line;
    if(mnem == JUMP || mnem == JINEG || mnem == JIZERO ){
      // Should have a label.
	auto lab = std::get<std::string>(line.operandV());
      if(labadds.find(lab) == labadds.end()){
	std::ostringstream os;
	os << "Error: label, " << lab << ", is not known!";
	throw std::runtime_error(os.str());
      }
      else{
	auto x = labadds[lab];
	std::cout << "\nlabadds[lab] is " << x;
	auto& op = line.operandV();
	op = static_cast<std::size_t>(x);
	std::cout << "\nLine: " << line;
      }
    }
  }
  return labadds;
}
