#pragma once
// (setq flycheck-clang-language-standard "c++11")
#include <vector>
#include <array>
#include <unordered_map>
#include <boost/unordered_map.hpp>
#include <unordered_set>

// for debug
#include <iostream>

struct AutomatonState {
  bool isMatch = false;
  AutomatonState () : isMatch(false) {}
  AutomatonState (bool isMatch) : isMatch(isMatch) {}
};

template<class Alphabet, class FinalState>
struct NonDeterministicAutomatonState : public AutomatonState {
  virtual void next(const Alphabet&, std::vector<std::shared_ptr<FinalState>>&) const = 0;
  NonDeterministicAutomatonState () : AutomatonState(false) {}
  NonDeterministicAutomatonState (bool isMatch) : AutomatonState(isMatch) {}
};

struct NFAState : public NonDeterministicAutomatonState<unsigned char, NFAState> {
  using ParentState = NonDeterministicAutomatonState<unsigned char, NFAState>;
  std::unordered_map<unsigned char, std::vector<std::weak_ptr<NFAState>>> nextMap;
  NFAState () : ParentState(false), nextMap({}) {}
  NFAState (bool isMatch) : ParentState(isMatch), nextMap({}) {}
  NFAState (bool isMatch, std::unordered_map<unsigned char, std::vector<std::weak_ptr<NFAState>>> next) : ParentState(isMatch), nextMap(next) {}
  void next(const unsigned char& c, std::vector<std::shared_ptr<NFAState>>& vec) const {
    vec.clear();
    auto it = nextMap.find(c);
    if (it != nextMap.end()) {
      vec.reserve(it->second.size());
      for (const auto &s: it->second) {
        vec.push_back(s.lock());
      }
    }
  }
};

template<class Alphabet, class FinalState>
struct DeterministicAutomatonState : public AutomatonState {
  virtual std::shared_ptr<FinalState> next(const Alphabet&) = 0;
  DeterministicAutomatonState () : AutomatonState(false) {}
  DeterministicAutomatonState (bool isMatch) : AutomatonState(isMatch) {}
};

struct DFAState : public DeterministicAutomatonState<unsigned char, DFAState> {
  std::unordered_map<unsigned char, std::weak_ptr<DFAState>> nextMap;
  DFAState () : DeterministicAutomatonState<unsigned char, DFAState>(false), nextMap({}) {}
  DFAState (bool isMatch) : DeterministicAutomatonState<unsigned char, DFAState>(isMatch), nextMap({}) {}
  DFAState (bool isMatch, std::unordered_map<unsigned char, std::weak_ptr<DFAState>> next) : DeterministicAutomatonState<unsigned char, DFAState>(isMatch), nextMap(std::move(next)) {}

  std::shared_ptr<DFAState> next(const unsigned char& c) {
    auto it = nextMap.find(c);
    if (it == nextMap.end()) {
      return std::shared_ptr<DFAState>();
    } else {
      return it->second.lock();
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
//       for (const std::weak_ptr<NFAState> ptr: ns->next.at(c)) {
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
  std::vector<std::shared_ptr<State>> states;
  //! @brief The initial states of this automaton.
  std::vector<std::shared_ptr<State>> initialStates;

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
  std::list<std::shared_ptr<NFAState>> states;
  std::list<std::shared_ptr<NFAState>> initStates;
  // std::list<std::shared_ptr<NFAState>> states;
  // std::list<const std::shared_ptr<NFAState>> initStates;
  tmpNFA () {}
  tmpNFA (const std::string cs) {
    states.push_back(std::make_shared<NFAState>(false));
    states.push_back(std::make_shared<NFAState>(true));
    for (auto c : cs) {
      states.front()->nextMap[c] = { std::weak_ptr<NFAState>(states.back()) };
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
//         for (const std::weak_ptr<NFAState> ptr: conf->next.at(c)) {
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
