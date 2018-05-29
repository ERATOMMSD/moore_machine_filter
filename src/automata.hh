#pragma once
// (setq flycheck-clang-language-standard "c++14")
#include <vector>
#include <array>
#include <valarray>
#include <unordered_map>
#include <boost/unordered_map.hpp>
#include <unordered_set>

// for debug
#include <iostream>

#include "constraint.hh"

//! @brief A state of an automaton
struct AutomatonState {
  //! @brief The value is true if and only if the state is an accepting state.
  bool isMatch = false;
  AutomatonState () : isMatch(false) {}
  AutomatonState (bool isMatch) : isMatch(isMatch) {}
};

//! @brief A state of a non-deterministic automaton
template<class Alphabet, class Transition>
struct NonDeterministicAutomatonState : public AutomatonState {
  std::unordered_map<unsigned char, std::vector<Transition>> nextMap;
  //  NonDeterministicAutomatonState (bool isMatch = false) : AutomatonState(isMatch) {}
  NonDeterministicAutomatonState (bool isMatch = false, std::unordered_map<unsigned char, std::vector<Transition>> nextMap = {}) : AutomatonState(isMatch), nextMap(std::move(nextMap)) {}
};

//! @brief A state of an NFA
struct NFAState : public NonDeterministicAutomatonState<unsigned char, NFAState*> {
  using Transition = NFAState*;
  using ParentState = NonDeterministicAutomatonState<unsigned char, NFAState*>;
  using Alphabet = unsigned char;
  using ParentState::ParentState;
  //  NFAState (bool isMatch = false, std::unordered_map<unsigned char, std::vector<NFAState*>> nextMap = {}) : ParentState(isMatch, std::move(nextMap)) {}
};

struct TATransition;
//! @brief A state of a timed automaton 
struct TAState : public NonDeterministicAutomatonState<unsigned char, TATransition> {
  using Transition = TATransition;
  using ParentState = NonDeterministicAutomatonState<unsigned char, TATransition>;
  using ParentState::ParentState;
  //  TAState (bool isMatch = false, std::unordered_map<unsigned char, std::vector<TATransition>> nextMap = {}) : ParentState(isMatch, std::move(nextMap)) {}
};

//! @brief A transition of a timed automaton
struct TATransition {
  //! @brief The pointer to the target state.
  TAState* target;
  //! @brief The clock variables reset after this transition.
  std::vector<ClockVariables> resetVars;
  //! @brief The guard for this transition.
  std::vector<Constraint> guard;

  void operator=(TAState* p) {
    target = p;
  }
  operator TAState*() const {
    return target;
  }
};

template<class Alphabet, class FinalState>
struct DeterministicAutomatonState : public AutomatonState {
  virtual FinalState* next(const Alphabet&) = 0;
  DeterministicAutomatonState (bool isMatch = false) : AutomatonState(isMatch) {}
};

struct DFAState : public DeterministicAutomatonState<unsigned char, DFAState> {
  std::unordered_map<unsigned char, DFAState*> nextMap;
  DFAState (bool isMatch = false, std::unordered_map<unsigned char, DFAState*> next = {}) : 
    DeterministicAutomatonState<unsigned char, DFAState>(isMatch), nextMap(std::move(next)) {}

  DFAState* next(const unsigned char& c) {
    auto it = nextMap.find(c);
    if (it == nextMap.end()) {
      return nullptr;
    } else {
      return it->second;
    }
  }
};

struct DRTAState;
//! @brief A transition of a deterministic real-time automaton
struct DRTATransition {
  std::pair<double, bool> upperBound;
  //! @brief The pointer to the target state.
  DRTAState* target;

  void operator=(DRTAState* p) {
    target = p;
  }
  bool operator<=(double x) const {
    return upperBound.first <= x;
  }
  bool operator<(double x) const {
    static const auto op = upperBound.second ? lt : le;
    return op(upperBound.first, x);
  }
  bool operator>(double x) const {
    return upperBound.first > x;
  }
  bool operator>=(double x) const {
    return upperBound.first > x || (upperBound.first == x && upperBound.second);
  }
  DRTAState* lock() {
    return target;
  }
private:
  static bool lt (const double a, const double b) {
    return a < b;
  }
  static bool le (const double a, const double b) {
    return a <= b;
  }
};

//! @brief A state of a deterministic real-time automaton 
struct DRTAState : public DeterministicAutomatonState<std::pair<unsigned char, double>, DRTAState> {
  using ParentState = DeterministicAutomatonState<std::pair<unsigned char, double>, DRTAState>;
  using Alphabet = std::pair<unsigned char, double>;
  boost::unordered_map<unsigned char, std::vector<DRTATransition>> nextMap;
  DRTAState (bool isMatch = false, boost::unordered_map<unsigned char, std::vector<DRTATransition>> next = {}) : 
    ParentState(isMatch), nextMap(std::move(next)) {}

  DRTAState* next(const std::pair<unsigned char, double>& c) {
    auto it = nextMap.find(c.first);
    if (it == nextMap.end()) {
      return nullptr;
    } else {
      // This takes O(log(N)) where N is the size of the partition.
      auto it2 = std::lower_bound(it->second.begin(), it->second.end(), c.second);
      return it2->lock();
    }
  }
};


// struct DFAState {
//   bool isMatch;
//   const std::vector<std::shared_ptr<NFAState>> toNFA;
//   DFAState(bool isMatch, const std::vector<std::shared_ptr<NFAState>> toNFA) : isMatch(isMatch), toNFA(toNFA) {
// #ifdef DEBUG
//     assert(!toNFA.empty());
// #endif // DEBUG
//   }
//   DFAState* next(const char c, boost::unordered_map<std::vector<std::shared_ptr<NFAState>>, DFAState*> &toDFA, std::deque<DFAState> &DFAStates) {
//     if (checked.at(c))
//       return nextArray.at(c);
//     checked.at(c) = true;

//     // Construct the next state in NFA
//     std::vector<std::shared_ptr<NFAState>> nfaNext;
// #ifdef DEBUG
//     assert(!toNFA.empty());
// #endif // DEBUG
//     for (std::shared_ptr<NFAState> ns: toNFA) {
// #ifdef DEBUG
//       assert(!toNFA.empty());
// #endif // DEBUG
//       nfaNext.reserve(nfaNext.size() + ns->next.at(c).size());
//       std::vector<std::shared_ptr<NFAState>> tmpNext;
//       tmpNext.reserve(ns->next.at(c).size());
//       for (const NFAState* ptr: ns->next.at(c)) {
//         if (!ptr.expired()) {
//           tmpNext.push_back(ptr.lock());
//         }
//       }
//       nfaNext.insert(nfaNext.end(), tmpNext.begin(), tmpNext.end());
//     }
//     if (nfaNext.empty()) return nullptr;
//     std::sort(nfaNext.begin(), nfaNext.end());
//     nfaNext.erase(std::unique(nfaNext.begin(), nfaNext.end()), nfaNext.end());
//     auto it = toDFA.find(nfaNext);
//     // There exists such DFA state
//     if (it != toDFA.end()) {
//       nextArray.at(c) = it->second;
//       return it->second;
//     }
//     // Construct a new state in DFA
//     DFAStates.emplace_back(std::any_of(nfaNext.begin(), nfaNext.end(), [](std::shared_ptr<NFAState> ps) {return ps->isMatch;}), // isMatch
//                            nfaNext /* toNFA */ );
//     nextArray.at(c) = &(DFAStates.back());
//     return toDFA[nfaNext] = &(DFAStates.back());
//   }
// private:
//   std::array<bool, CHAR_MAX> checked = {};
//   std::array<DFAState*, CHAR_MAX> nextArray = {};
// };

template<class State>
struct Automaton {
  //! @brief The states of this automaton.
  std::vector<State*> states;
  //! @brief The initial states of this automaton.
  std::vector<State*> initialStates;

  //! @brief Returns the number of the states.
  inline std::size_t stateSize() const {return states.size ();}

  inline bool operator == (const Automaton<State> A) const {
    return initialStates == A.initialStates &&
      states == A.states;
  }
};

// struct DFA {
//   std::deque<DFAState> states;
//   std::size_t init;
// };

// Probably shared_ptr based implementation is better
struct tmpNFA {
  std::list<NFAState*> states;
  std::list<NFAState*> initStates;
  // std::list<std::shared_ptr<NFAState>> states;
  // std::list<const std::shared_ptr<NFAState>> initStates;
  tmpNFA () {}
  tmpNFA (const std::string cs) {
    states.push_back(new NFAState(false));
    states.push_back(new NFAState(true));
    for (auto c : cs) {
      states.front()->nextMap[c] = { (NFAState*)(states.back()) };
    }
    initStates = { states.front() };
  }
};

using NFA = Automaton<NFAState>;

// struct NFA {
//   std::vector<std::shared_ptr<NFAState>> states;
//   std::vector<std::shared_ptr<NFAState>> initStates;
//   NFA () {}
//   NFA (const std::vector<std::shared_ptr<NFAState>>& states, const std::vector<std::shared_ptr<NFAState>>& initStates) : states(std::move(states)), initStates(initStates) {}
//   NFA (tmpNFA& in) : states(in.states.begin(), in.states.end()), initStates(in.initStates.begin(), in.initStates.end()) { }
//   // boost::unordered_map<std::vector<std::shared_ptr<NFAState>>, DFAState*> toDFA;

//   // @note This function is only for debugging
//   bool isMatch(const std::string &str) {
//     std::vector<std::shared_ptr<NFAState>> currentConf = initStates;    
//     for (auto c: str) {
//       if (currentConf.empty()) {
//         return false;
//       }
//       std::vector<std::shared_ptr<NFAState>> nextConf;
//       for (auto conf: currentConf) {
//         std::vector<std::shared_ptr<NFAState>> tmpNext;
//         for (const NFAState* ptr: conf->next.at(c)) {
//           if (!ptr.expired()) {
//             tmpNext.push_back(ptr.lock());
//           }
//         }
//         nextConf.insert(nextConf.end(), tmpNext.begin(), tmpNext.end());
//       }
//       currentConf = std::move(nextConf);
//     }
//     return std::any_of(currentConf.begin(), currentConf.end(), [](const std::shared_ptr<NFAState> s) {
//         return s->isMatch;
//       });
//   }
// };

struct TimedAutomaton : public Automaton<TAState> {
  using X = ConstraintMaker;
  using State = ::TAState;

  //! @brief The maximum constraints for each clock variables.
  std::vector<int> maxConstraints;
  /*!
    @brief make a deep copy of this timed automaton.

    @param [out] dest The destination of the deep copy.
    @param [out] old2new The mapping from the original state to the corresponding new state.
   */
  void deepCopy(TimedAutomaton& dest, std::unordered_map<TAState*, TAState*> &old2new) const {
    // copy states
    old2new.reserve(stateSize());
    dest.states.reserve(stateSize());
    for (auto oldState: states) {
      dest.states.emplace_back(new TAState(*oldState));
      old2new[oldState] = dest.states.back();
    }
    // copy initial states
    dest.initialStates.reserve(initialStates.size());
    for (auto oldInitialState: initialStates) {
      dest.initialStates.emplace_back(old2new[oldInitialState]);
    }
    // modify dest of transitions
    for (auto &state: dest.states) {
      for (auto &edges: state->nextMap) {
        for (auto &edge: edges.second) {
          auto oldTarget = edge.target;
          edge.target = old2new[oldTarget];
        }
      }

    }
    dest.maxConstraints = maxConstraints;
  }
  //! @brief Returns the number of clock variables used in the timed automaton.
  inline size_t clockSize() const {return maxConstraints.size ();}

  /*!
    @brief solve membership problem for observable timed automaton
    @note This is only for testing.
    @note If there are epsilon transitions, this does not work.
   */
  bool isMember(const std::vector<std::pair<unsigned char, double>> &w) const {
    std::vector<std::pair<TAState*, std::valarray<double>>> CStates;
    CStates.reserve(initialStates.size());
    for (const auto& s: initialStates) {
      CStates.emplace_back(s, std::valarray<double>(0.0, clockSize()));
    }
    for (std::size_t i = 0; i < w.size(); i++) {
      std::vector<std::pair<TAState*, std::valarray<double>>> NextStates;
      for (std::pair<TAState*, std::valarray<double>> &config: CStates) {
        if (i > 0) {
          config.second += w[i].second - w[i-1].second;
        } else {
          config.second += w[i].second;
        }
        auto it = config.first->nextMap.find(w[i].first);
        if (it == config.first->nextMap.end()) {
          continue;
        }
        for (const auto &edge: it->second) {
          if (std::all_of(edge.guard.begin(), edge.guard.end(), [&](const Constraint &g) {
                return g.satisfy(config.second[g.x]);
              })) {
            auto tmpConfig = config;
            tmpConfig.first = edge.target;
            if (tmpConfig.first) {
              for (ClockVariables x: edge.resetVars) {
                tmpConfig.second[x] = 0; 
              }
              NextStates.emplace_back(std::move(tmpConfig));
            }
          }
        }
      }
      CStates = std::move(NextStates);
    }
    return std::any_of(CStates.begin(), CStates.end(), [](std::pair<const TAState*, std::valarray<double>> p) {
        return p.first->isMatch;
      });
  }
};

namespace std {
  template<class T, class U>
  static inline std::ostream &operator<<(std::ostream &os, const std::pair<T, U> &b) {
    os << "(" << b.first << ", " << b.second << ")";
    return os;
  }
};
