#pragma once

#include <queue>
#include <numeric>
// (setq flycheck-clang-language-standard "c++14")
#include "automata.hh"
#include "add_counter.hh"
#include "bit_buffer.hh"


template<class T>
constexpr const T maskChar;

template<>
constexpr const unsigned char maskChar<unsigned char> = '_';

template<>
constexpr const std::pair<unsigned char, double> maskChar<std::pair<unsigned char, double>> = {'_', 0};

template<class T>
constexpr T mask(T) {
  return maskChar<T>;
}

template<>
constexpr inline std::pair<unsigned char, double> mask(std::pair<unsigned char, double> c) {
  return std::make_pair(maskChar<unsigned char>, c.second);
}

template<int BufferSize, class Alphabet, class State>
struct MooreMachine : public Automaton<State> {
  std::unordered_map<State*, std::size_t> counter;
  std::queue<Alphabet> charBuffer;
  BitBuffer<BufferSize> bitBuffer;
  State* currentState;
  MooreMachine() {
    for (int i = 0; i < BufferSize; i++) {
      charBuffer.push(maskChar<Alphabet>);
    }
  }

  Alphabet feed(Alphabet c) {
    currentState = currentState->next(c);
    if (!currentState) {
      currentState = Automaton<State>::initialStates[0];
    }
    const Alphabet ret = 
      bitBuffer.getAndEnable(counter[currentState]) ? 
      charBuffer.front() : mask(charBuffer.front());
    charBuffer.pop();
    charBuffer.push(c);

    return ret;
  }
};

// NFAWithCounter -> MooreMachine
template<int BufferSize>
void toMooreMachine(NFAWithCounter<BufferSize> &from, MooreMachine<BufferSize, unsigned char, DFAState> &to) {
  // S_{old} -> Int -> S_{new}
  boost::unordered_map<std::vector<NFAState*>, DFAState*> toNewState;
  boost::unordered_map<DFAState*, std::vector<NFAState*>> toOldStates;
  std::vector<DFAState*> currStates;

  const auto addNewState = [&](const std::vector<NFAState*> &s) -> DFAState* {
    // isMatch
    auto newState = new DFAState(std::any_of(s.begin(), s.end(), [](NFAState* ps) {
          return ps->isMatch;
        }));
    toNewState[s] = newState;
    toOldStates[newState] = s;
    to.counter[newState] = std::accumulate(s.begin(), s.end(), 0, [&](int x, NFAState* ps) {
        if (from.counter[ps] == BufferSize) {
          return BufferSize;
        } else if (ps->isMatch) {
          return std::max<int>(x, from.counter[ps]);
        } else {
          return x;
        }
      });
    to.states.push_back(newState);
    currStates.push_back(newState);
    return newState;
  };

  // add initial state
  to.initialStates = {addNewState(from.initialStates)};
  to.currentState = to.initialStates[0];

  while (!currStates.empty()) {
    std::vector<DFAState*> prevStates = std::move(currStates);
    currStates.clear();
    for (auto s: prevStates) {
      // merge the next unordered_map of each NFAState
      std::unordered_map<unsigned char, std::vector<NFAState*>> mergedNext;
      for (auto vecs: toOldStates[s]) {
        for (const auto &nextPair: vecs->nextMap) {
          mergedNext[nextPair.first].reserve(mergedNext[nextPair.first].size() + nextPair.second.size());
          for (NFAState* ws: nextPair.second) {
            mergedNext[nextPair.first].push_back(ws);
          }
        }
      }
      for (auto &nextPair: mergedNext) {
        nextPair.second.insert(nextPair.second.end(), from.initialStates.begin(), from.initialStates.end());
        std::sort(nextPair.second.begin(), nextPair.second.end());
        nextPair.second.erase(std::unique(nextPair.second.begin(), nextPair.second.end()), nextPair.second.end());
      }

      // add a transition to the next DFAState
      for (auto nextPair: mergedNext) {
        DFAState* newTarget;

        auto it = toNewState.find(nextPair.second);
        if (it == toNewState.end()) {
          // When this state is new
          // construct the next DFAState
          newTarget = addNewState(nextPair.second);
        } else {
          // When this state is not new
          newTarget = it->second;
        }

        s->nextMap[nextPair.first] = std::move(newTarget);
      }
    }
  }

}
