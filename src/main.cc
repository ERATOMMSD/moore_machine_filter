#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/exception/diagnostic_information.hpp> 

#include "automata.hh"
#include "timed_automaton_parser.hh"
#include "moore_machine.hh"
#include "timed_moore_machine.hh"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 10
#endif

using namespace boost::program_options;

static inline int getOne(FILE* file, unsigned char &c) {
  return fscanf(file, " %c\n", &c);
}

static inline int getOne(FILE* file, std::pair<unsigned char, double> &p) {
  return fscanf(file, " %c %lf\n", &p.first, &p.second);
}

static inline int putOne(FILE* file, const unsigned char &c) {
  return fprintf(file, "%c\n", c);
}

static inline int putOne(FILE* file, const std::pair<unsigned char, double> &p) {
  return fprintf(file, "%c %lf\n", p.first, p.second);
}

template<int BufferSize>
void toMooreMachine(const NFA &, NFAWithCounter<BufferSize> &counter, MooreMachine<BufferSize, unsigned char, DFAState> &filterMachine) {
  toMooreMachine(counter, filterMachine);
}

template<int BufferSize, class Alphabet, class FiltState>
void toMooreMachine(const TimedAutomaton &TA, const TAWithCounter<BufferSize> counter, MooreMachine<BufferSize, Alphabet, FiltState> &filterMachine) {
  Bounds M = {static_cast<double>(*std::max_element(TA.maxConstraints.begin(), TA.maxConstraints.end())), true};
  toTimedMooreMachine(counter, M, TA.clockSize(), filterMachine);
}

template<class State, class Autom, int BufferSize, class Alphabet, class FiltState>
void constructFilter(Autom &TA, MooreMachine<BufferSize, Alphabet, FiltState> &filterMachine) {
  AutomatonWithCounter<State, BufferSize> counter;
  toAutomatonWithCounter(TA, counter);
  toMooreMachine(TA, counter, filterMachine);
}

template<int BufferSize, class Alphabet, class State>
void filter(MooreMachine<BufferSize, Alphabet, State> &A, FILE *fin, FILE* fout) {
  Alphabet c;
  std::size_t counter = 0;
  while(getOne(fin, c) != EOF) {
    if (counter >= BufferSize) {
      putOne(fout, A.feed(c));
    } else {
      A.feed(c);
      counter++;
    }
  }
  std::size_t finCounter = counter;
  while (counter < BufferSize) {
    A.feed(maskChar<Alphabet>);
    counter++;
  }
  while(finCounter > 0) {
    putOne(fout, A.feed(maskChar<Alphabet>));
    finCounter--;
  }
}

void operator>>(std::ifstream &stream, TimedAutomaton &A) {
  BoostTimedAutomaton BoostTA;
  parseBoostTA(stream, BoostTA);
  convBoostTA(BoostTA, A);
}

template<class State>
void operator>>(std::ifstream &stream, Automaton<State> &A) {
  BoostTimedAutomaton BoostTA;
  parseBoostTA(stream, BoostTA);
  convBoostTA(BoostTA, A);
}

int main(int argc, char *argv[])
{
  const auto programName = "filt";
  const auto errorHeader = "filt: ";

  const auto die = [&errorHeader] (auto message, int status) {
    std::cerr << errorHeader << message << std::endl;
    exit(status);
  };

  // visible options
  options_description visible("description of options");
  std::string automatonFileName;
  bool isTimed = false;
  visible.add_options()
    ("help,h", "help")
    ("version,V", "version")
    ("automaton,f", value<std::string>(&automatonFileName)->default_value(""),"input file of Timed Automaton")
    ("untimed,u", "untimed mode (default)")
    ("timed,t", "timed mode");

  command_line_parser parser(argc, argv);
  variables_map vm;
  parser.options(visible);
  try {
    store(parser.run(), vm);
  } catch (std::exception &e) {
    die(e.what(), 1);
  }
  notify(vm);

  if (automatonFileName.empty() || vm.count("help")) {
    std::cout << programName << " [OPTIONS] -f FILE\n" 
              << visible << std::endl;
    return 0;
  }
  if (vm.count("version")) {
    std::cout << "filt 0.1\n"
              << visible << std::endl;
    return 0;
  }
  if ((vm.count("timed") && vm.count("untimed"))) {
    die("conflicting input formats specified", 1);
  }

  if (vm.count("timed")) {
    isTimed = true;
  } else if (vm.count("untimed")) {
    isTimed = false;
  }

  std::ifstream automatonStream(automatonFileName);
  if (isTimed) {
    TimedAutomaton TA;
    // parse TA
    automatonStream >> TA;
    MooreMachine<BUFFER_SIZE, std::pair<unsigned char, double>, DRTAState> filterMachine;
    constructFilter<TAState>(TA, filterMachine);
    filter(filterMachine, stdin, stdout);
  } else {
    // parse NFA
    NFA A;
    automatonStream >> A;
    MooreMachine<BUFFER_SIZE, unsigned char, DFAState> filterMachine;
    constructFilter<NFAState>(A, filterMachine);
    filter(filterMachine, stdin, stdout);
  }

  return 0;
}
