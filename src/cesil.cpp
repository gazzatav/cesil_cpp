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
    in_ >> std::ws;
    char look_ahead = in_.peek();
    if(look_ahead == '%'){
      in_.ignore(1000, '\n');
      return StartLine{};
    }
    else if(isDigit(look_ahead)){
      in_.ignore(1000, '\n');
      return StartLine{};
    }

    else if(look_ahead == '('){
      in_.ignore(1000, '\n');
      return StartLine{};
    }
    else if(look_ahead == '*'){
      in_.ignore(1000, '\n');
      return StartLine{};
    }
    std::string token;
    in_ >> token;
    if(auto mnemonic = strToMnemonic(token); mnemonic == Mnemonic::UNKNOWN){
      col1 = token;
      return LabelFound{};
    }
    else{
      col2 = mnemonic;
      return MnemonicFound{};
    }
  }

  std::variant<StartLine, LabelFound, MnemonicFound, OperandFound, LineDone>
  operator()(LabelFound&) {
    if(remainsNewLine(in_)){
      in_ >> std::ws;
      col2 = Mnemonic::NOOP;
      return LineDone{};
    }
    std::string token;
    in_ >> token;
    if(auto mnemonic = strToMnemonic(token); mnemonic != Mnemonic::UNKNOWN){
      col2 = mnemonic;
      return MnemonicFound{};
    }
    else if(token.size() > 0){
      throw std::runtime_error("Extra label after label (no mnemonic).");
    }
    else{
      throw std::runtime_error("Invalid Program Line in operator()(LabelFound).");
    }
  }

  std::variant<StartLine, LabelFound, MnemonicFound, OperandFound, LineDone>
  operator()(MnemonicFound&) {
    if(remainsNewLine(in_)){
      return LineDone{};
    }
    in_ >> std::ws;
    char look_ahead = in_.peek();
    if(look_ahead == '"'){
      in_.ignore(1);
      std::string literal;
      std::getline(in_, literal, '"');
      col3 = literal;
      return OperandFound{};
    }
    else if(isDigit(look_ahead)){
      int digits;
      in_ >> digits;
      col3 = digits;
      return OperandFound{};
    }
    else if(look_ahead > 0x20 && look_ahead < 0x7b){
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
    while(in_.peek() == ' ' || in_.peek() == '\t'){
      char c;
      in_ >> std::noskipws >> c;
    }
    char p = in_.peek();
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
    in_.clear();
    in_ >> std::ws;
    return StartLine{};
  }
};

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
  in.clear();
  in.seekg(0, in.beg);
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
  Data data;
  in.clear();
  in.seekg(0, in.beg);
  while(in.good()){
    char c = in.peek();
    if(c == '%'){
      in.ignore(1000, '\n');
      while(in.good() && c != '*'){
	int32_t d;
	in >> d;
	in.ignore(1000, '\n');
	c = in.peek();
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
  for(auto& line: prog){
    using enum Mnemonic;
    auto mnem = std::get<Mnemonic>(line.mnemonicV()); // every prog line has mnemonic.
    if(mnem == JUMP || mnem == JINEG || mnem == JIZERO ){
	auto lab = std::get<std::string>(line.operandV());
      if(labadds.find(lab) == labadds.end()){
	std::ostringstream os;
	os << "Error: label, " << lab << ", is not known!";
	throw std::runtime_error(os.str());
      }
      else{
	auto x = labadds[lab];
	auto& op = line.operandV();
	op = static_cast<std::size_t>(x);
      }
    }
  }
  return labadds;
}

CesilMachine::CesilMachine(Program& prog, Data& data, NamedVars& vars)
  :prog_(std::move(prog)), data_(std::move(data)), vars_(std::move(vars)), pc(0), dc(0), accumulator(0){}

void CesilMachine::executeLine(){
  switch(std::get<Mnemonic>(prog_[pc].mnemonicV())){
    using enum Mnemonic;
  case NOOP:
    ++pc;
    break;
  case PRINT:
    std::cout << std::get<std::string>(prog_[pc].operandV());
    ++pc;
    break;
  case LINE:
    std::cout << '\n';
    ++pc;
    break;
  case IN:
    if(dc < data_.size()){
      accumulator = data_[dc];
      ++dc; ++pc;
    }
    else{
      std::cout << "\n*** PROGRAM REQUIRES MORE DATA ***" << std::endl;
      run_ = false;
    }
    break;
  case OUT:
    std::cout << accumulator;
    ++pc;
    break;
  case LOAD:
    if(std::holds_alternative<std::string>(prog_[pc].operandV())){
      auto name = std::get<std::string>(prog_[pc].operandV());
      accumulator = static_cast<int32_t>(vars_[name]);
      }
    else{
      accumulator = std::get<int32_t>(prog_[pc].operandV());
    }
    ++pc;
    break;
  case STORE:
    if(std::holds_alternative<std::string>(prog_[pc].operandV())){
      vars_[std::get<std::string>(prog_[pc].operandV())] = accumulator;
    }
    else{
      throw std::runtime_error("Error: Program lacked a store location.");
    }
    ++pc;
    break;
  case ADD:
    if(std::holds_alternative<std::string>(prog_[pc].operandV())){
      accumulator += vars_[std::get<std::string>(prog_[pc].operandV())];
    }
    else{
      accumulator += std::get<int32_t>(prog_[pc].operandV());
    }
    ++pc;
    break;
  case SUBTRACT:
    if(std::holds_alternative<std::string>(prog_[pc].operandV())){
      accumulator -= vars_[std::get<std::string>(prog_[pc].operandV())];
    }
    else{
      accumulator -= std::get<int32_t>(prog_[pc].operandV());
    }
    ++pc;
    break;
  case MULTIPLY:
    if(std::holds_alternative<std::string>(prog_[pc].operandV())){
      accumulator *= vars_[std::get<std::string>(prog_[pc].operandV())];
    }
    else{
      accumulator *= std::get<int32_t>(prog_[pc].operandV());
    }
    ++pc;
    break;
  case DIVIDE:
    int32_t divisor;
    if(std::holds_alternative<std::string>(prog_[pc].operandV())){
      divisor = vars_[std::get<std::string>(prog_[pc].operandV())];
    }
    else{
      divisor = std::get<int32_t>(prog_[pc].operandV());
    }
    if(divisor == 0){
      std::cout << "\n*** DIVISION BY ZERO ***" << std::endl;
      run_ = false;
    }
    else{
      accumulator /= divisor;
    }
    ++pc;
    break;
  case JUMP:
    if(std::holds_alternative<std::size_t>(prog_[pc].operandV())){
      pc = std::get<std::size_t>(prog_[pc].operandV());
    }
    else{
      throw std::runtime_error("JUMP instruction without label");
    }
    break;
  case JINEG:
    if(std::holds_alternative<std::size_t>(prog_[pc].operandV())){
      if(accumulator < 0){
	pc = std::get<std::size_t>(prog_[pc].operandV());
      }
      else{
	++pc;
      }
    }
    else{
      throw std::runtime_error("JINEG instruction without label");
    }
    break;
  case JIZERO:
    if(std::holds_alternative<std::size_t>(prog_[pc].operandV())){
      if(accumulator == 0){
      pc = std::get<std::size_t>(prog_[pc].operandV());
      }
      else{
	++pc;
      }
    }
    else{
      throw std::runtime_error("JIZERO instruction without label");
    }
    break;
  case HALT:
    run_ = false;
    break;
  default:++pc;
  }
}
void CesilMachine::run(){
  run_ =  true;
  std::cout << "Running with PC = " << pc << '\n';
  while(run_){
    executeLine();
  }
}

Program& CesilMachine::eject(){
  return prog_;
}

void CesilMachine::debug(){
    run_ =  true;
    std::cout << std::string(10, '*') << " DEBUG " << std::string(10, '*');
    std::cout << "\nData: ";
    for(auto d: data_)
      std::cout << d << ",\t";
    for(auto& var: vars_){
      std::cout << "Name: " << var.first << ", Val: " << var.second << '\n';
    }

  while(run_){
    for(auto& var: vars_){
      std::cout << "Name: " << var.first << ", Val: " << var.second << '\n';
    }
    std::cout << "Debug: PC: " << pc << ", DC: " << dc << ", ACC: " << accumulator << '\n';
    std::cout << "LINE: " << prog_[pc] << '\n' << std::string(80, '=') << '\n';
    executeLine();
    std::cout << '\n' << std::string(80,'=') << "\nDebug: PC: " << pc << ", DC: " << dc<< ", ACC: " << accumulator << '\n';
    for(auto& var: vars_){
      std::cout << "Name: " << var.first << ", Val: " << var.second << "\n\n";
    }
  }
      std::cout << std::string(10, '*') << " DEBUG END " << std::string(10, '*');
}
void CesilMachine::reset(){
  pc = 0;
  dc = 0;
  accumulator = 0;
}
