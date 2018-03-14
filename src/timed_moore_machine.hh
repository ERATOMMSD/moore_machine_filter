#pragma once
#include <boost/unordered_set.hpp>

// (setq flycheck-clang-language-standard "c++14")
#include "dbm.hh"
#include "moore_machine.hh"

void constructNexts(const std::shared_ptr<DRTAState> &s,
                    std::unordered_map<std::shared_ptr<DRTAState>, std::unordered_multimap<std::shared_ptr<TAState>, DBM>> &toOldStates, 
                    boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> &nexts);

void linearlizeNexts(const boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> &nexts,
                     const std::vector<std::pair<std::shared_ptr<TAState>, DBM>> &initConfs,
                     std::unordered_map<unsigned char, std::vector<std::pair<std::unordered_multimap<std::shared_ptr<TAState>, DBM>, Bounds>>> &nextLineared);

// TAWithCounter -> TimedMooreMachine
template<int BufferSize>
void toTimedMooreMachine(const TAWithCounter<BufferSize> &from, const Bounds &M, const std::size_t numOfVariables, MooreMachine<BufferSize, std::pair<unsigned char, double>, DRTAState> &to) 
{
  // S_{old} -> Int -> S_{new}
  // Q = S \times Z(X)
  boost::unordered_map<std::unordered_multimap<std::shared_ptr<TAState>, DBM>, std::shared_ptr<DRTAState>> toNewState;
  std::unordered_map<std::shared_ptr<DRTAState>, std::unordered_multimap<std::shared_ptr<TAState>, DBM>> toOldStates;
  std::vector<std::shared_ptr<DRTAState>> currStates;

  const auto addNewState = [&](const std::unordered_multimap<std::shared_ptr<TAState>, DBM> &s) -> std::shared_ptr<DRTAState> {
    auto newState = std::make_shared<DRTAState>(std::any_of(s.begin(), s.end(), [](std::pair<std::shared_ptr<TAState>, DBM> ps) {
          // isMatch
          return ps.first->isMatch;
        }));
    toNewState[s] = newState;
    toOldStates[newState] = s;
    to.counter[newState] = std::accumulate(s.begin(), s.end(), 0, [&](int x, std::shared_ptr<NFAState> ps) {
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
  std::vector<std::pair<std::shared_ptr<TAState>, DBM>> initConfs;
  {
    DBM initZone = DBM::zero(numOfVariables + 1);
    initZone.M = M;
    initConfs.reserve(from.initialStates.size());
    for (const auto &s: from.initialStates) {
      initConfs.emplace_back(s, initZone);
    }
    std::sort(initConfs.begin(), initConfs.end());
    to.initialStates = {addNewState(initConfs)};
    to.currentState = to.initialStates[0];
  }

  while (!currStates.empty()) {
    std::vector<std::shared_ptr<DRTAState>> prevStates = std::move(currStates);
    currStates.clear();
    for (auto s: prevStates) {
      // merge the next unordered_map of each DRTAState
      boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> nexts;

      // NOTE: HERE IS THE DIFFICULT POINT!!!!
      constructNexts(s, toOldStates, nexts);

      std::unordered_map<unsigned char, std::vector<std::pair<std::unordered_multimap<std::shared_ptr<TAState>, DBM>, Bounds>>> nextLineared;
      linearlizeNexts(nexts, initConfs, nextLineared);

      // add a transition to the next DTRAState
      for (const auto &nextPair: nextLineared) {
        s->nextMap[nextPair.first].reserve(nextPair.second.size());

        for (auto targetPair: nextPair.second) {
          auto it = toNewState.find(targetPair.first);
          if (it == toNewState.end()) {
            // When this state is new
            // construct the next DTRAState
            s->nextMap[nextPair.first].emplace_back(addNewState(targetPair.first), targetPair.second);
          } else {
            // When this state is not new
            s->nextMap[nextPair.first].emplace_back(it->second, targetPair.second);
          }
        }
      }
    }
  }

}
