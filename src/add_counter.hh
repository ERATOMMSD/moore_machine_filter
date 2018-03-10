#pragma once

#include "automata.hh"

template<int BufferSize>
struct NFAWithCounter : public NFA {
  std::unordered_map<std::shared_ptr<NFAState>, std::size_t> counter;
};

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
  for (const auto &s: from.initStates) {
    to.initStates.push_back(addNewState(s, 0));
  }
  std::sort(to.initStates.begin(), to.initStates.end());

  while (!currStates.empty()) {
    std::vector<std::shared_ptr<NFAState>> prevStates = std::move(currStates);
    currStates.clear();

    for (auto s: prevStates) {
      const std::size_t count = to.counter[s];
      const std::size_t nextCount = (count == BufferSize) ? 1 : (count + 1);
      auto oldNext = std::move(s->next);
      s->next.clear();
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

        s->next[nextPair.first] = std::move(newTargets);
      }
    }
  }
}
