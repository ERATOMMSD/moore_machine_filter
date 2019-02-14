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

template<class T, int BufferSize>
class ConstQueue 
{
private:
  std::array<T, BufferSize> data;
  std::size_t fpos = 0, bpos = -1;
  std::size_t shift_front () {
    return fpos = (fpos + 1) % BufferSize;
  }
  std::size_t shift_back () {
    return bpos = (bpos + 1) % BufferSize;
  }
public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  reference front() {
    return data[fpos];
  }
  const_reference front() const {
    return data[fpos];
  }
  reference back() {
    return data[bpos];
  }
  const_reference back() const {
    return data[bpos];
  }
  void push_back(const T& x) {
    data[shift_back()] = std::move(x);
  }
  void push_back(T&& x) {
    data[shift_back()] = std::move(x);
  }
  void pop_front() {
    shift_front();
  }
};

/*!
  @brief Moore Machine (both untimed and timed)
 */
template<int BufferSize, class Alphabet, class State>
struct MooreMachine : public Automaton<State> {
  //  std::unordered_map<State*, std::size_t> counter;
  //! @brief Buffer for the read characters
  std::queue<Alphabet, ConstQueue<Alphabet, BufferSize>> charBuffer;
  /*! 
    @brief Buffer for the pass/mask flag

    - true :: pass
    - false :: mask
  */
  BitBuffer<BufferSize> bitBuffer;
  //! @brief Current state of the Moore machine
  State* currentState;
  /*!
    @brief Constructor
    @note This function initialize the buffer.
   */
  MooreMachine() {
    for (int i = 0; i < BufferSize; i++) {
      charBuffer.push(maskChar<Alphabet>);
    }
  }

  /*!
    @brief Consumes one character and returns the poped character
    @param [in] c The current character
    @retval The masked or unmasked character.
    @note We assume that the number of the initial states is one.
   */
  Alphabet feed(Alphabet c) {
    currentState = reinterpret_cast<State*>(currentState->next(c));
    if (!currentState) {
      currentState = Automaton<State>::initialStates[0];
    }
    const Alphabet ret = 
      bitBuffer.getAndEnable(currentState->counter) ? 
      charBuffer.front() : mask(charBuffer.front());
    charBuffer.pop();
    charBuffer.push(c);

    return ret;
  }
};

// NFAWithCounter -> MooreMachine
template<int BufferSize>
void toMooreMachine(NFAWithCounter<BufferSize> &from, MooreMachine<BufferSize, unsigned char, DFAStateWithCounter> &to) {
  // S_{old} -> Int -> S_{new}
  boost::unordered_map<std::vector<NFAState*>, DFAStateWithCounter*> toNewState;
  boost::unordered_map<DFAStateWithCounter*, std::vector<NFAState*>> toOldStates;
  std::vector<DFAStateWithCounter*> currStates;

  const auto addNewState = [&](const std::vector<NFAState*> &s) -> DFAStateWithCounter* {
    // isMatch
    auto newState = new DFAStateWithCounter(std::any_of(s.begin(), s.end(), [](NFAState* ps) {
          return ps->isMatch;
        }));
    toNewState[s] = newState;
    toOldStates[newState] = s;
    newState->counter = std::accumulate(s.begin(), s.end(), 0, [&](int x, NFAState* ps) {
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
    std::vector<DFAStateWithCounter*> prevStates = std::move(currStates);
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
