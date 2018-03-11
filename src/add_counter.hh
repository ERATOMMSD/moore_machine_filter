#pragma once

#include "automata.hh"

template<class T, int BufferSize>
struct AutomatonWithCounter : public Automaton<T> {
  std::unordered_map<std::shared_ptr<T>, std::size_t> counter;
};

template<int BufferSize>
using NFAWithCounter = AutomatonWithCounter<NFAState, BufferSize>;

// NFA -> Int -> NFAWithCounter
template<int BufferSize>
void toNFAWithCounter(const NFA &from, NFAWithCounter<BufferSize> &to) {
  // S_{old} -> Int -> S_{new}
  boost::unordered_map<std::pair<std::shared_ptr<NFAState>, std::size_t>, std::shared_ptr<NFAState>> toNewState;
  std::vector<std::shared_ptr<NFAState>> currStates;

  const auto addNewState = [&](std::shared_ptr<NFAState> s, std::size_t c) -> std::shared_ptr<NFAState> {
    auto newState = std::make_shared<NFAState>(*s);
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
    std::vector<std::shared_ptr<NFAState>> prevStates = std::move(currStates);
    currStates.clear();

    for (auto s: prevStates) {
      const std::size_t count = to.counter[s];
      const std::size_t nextCount = (count == BufferSize) ? 1 : (count + 1);
      // TODO: swap target
      auto oldNext = std::move(s->nextMap);
      s->nextMap.clear();
      for (auto nextPair: oldNext) {
        std::vector<std::weak_ptr<NFAState>> newTargets;
        newTargets.reserve(nextPair.second.size());
        for (std::weak_ptr<NFAState> nextS: nextPair.second) {
          const std::shared_ptr<NFAState> nextS_shared = nextS.lock();

          auto it = toNewState.find(std::make_pair(nextS_shared, nextCount));
          if (it == toNewState.end()) {
            // When this state is new
            newTargets.push_back(addNewState(nextS_shared, nextCount));
          } else {
            // When this state is not new
            newTargets.push_back(it->second);
          }
        }

        s->nextMap[nextPair.first] = std::move(newTargets);
      }
    }
  }
}
