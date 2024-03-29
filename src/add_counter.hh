#pragma once

#include "automata.hh"

template<class T, int BufferSize>
struct AutomatonWithCounter : public Automaton<T> {
  std::unordered_map<std::shared_ptr<T>, std::size_t> counter;
};

template<int BufferSize>
using NFAWithCounter = AutomatonWithCounter<NFAState, BufferSize>;

template<int BufferSize>
using TAWithCounter = AutomatonWithCounter<TAState, BufferSize>;

// NFA -> Int -> NFAWithCounter
template<class State, int BufferSize>
void toAutomatonWithCounter(const Automaton<State> &from, AutomatonWithCounter<State, BufferSize> &to) {
  // S_{old} -> Int -> S_{new}
  boost::unordered_map<std::pair<std::shared_ptr<State>, std::size_t>, std::shared_ptr<State>> toNewState;
  std::vector<std::shared_ptr<State>> currStates;

  const auto addNewState = [&](std::shared_ptr<State> s, std::size_t c) -> std::shared_ptr<State> {
    auto newState = std::make_shared<State>(*s);
    toNewState[std::make_pair(s, c)] = newState;
    to.counter[newState] = c;
    to.states.push_back(newState);
    currStates.push_back(newState);
    return newState;
  };

  // add initial states
  for (const auto &s: from.initialStates) {
    to.initialStates.push_back(addNewState(s, 0));
  }
  std::sort(to.initialStates.begin(), to.initialStates.end());

  while (!currStates.empty()) {
    std::vector<std::shared_ptr<State>> prevStates = std::move(currStates);
    currStates.clear();

    for (auto s: prevStates) {
      const std::size_t count = to.counter[s];
      const std::size_t nextCount = (count == BufferSize) ? 1 : (count + 1);
 
      for (auto &nextPair: s->nextMap) {
        for (typename State::Transition &nextS: nextPair.second) {
          const std::shared_ptr<State> nextS_shared = nextS.lock();

          auto it = toNewState.find(std::make_pair(nextS_shared, nextCount));
          if (it == toNewState.end()) {
            // When this state is new
            nextS = addNewState(nextS_shared, nextCount);
          } else {
            // When this state is not new
            nextS = it->second;
          }
        }
      }
    }
  }
}
