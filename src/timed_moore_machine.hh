#pragma once
#include <boost/unordered_set.hpp>

// (setq flycheck-clang-language-standard "c++14")
#include "dbm.hh"
#include "moore_machine.hh"

// Over-approximate a timed automaton by a real-time automaton
void constructNexts(const std::shared_ptr<DRTAState> &s,
                    std::unordered_map<std::shared_ptr<DRTAState>, std::unordered_multimap<std::shared_ptr<TAState>, DBM>> &toOldStates, 
                    boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> &nexts);

void linearlizeNexts(const boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> &nexts,
                     const std::unordered_multimap<std::shared_ptr<TAState>, DBM> &initConfs,
                     std::unordered_map<unsigned char, std::vector<std::pair<std::unordered_multimap<std::shared_ptr<TAState>, DBM>, Bounds>>> &nextLineared);

// TAWithCounter -> TimedMooreMachine
template<int BufferSize>
void toTimedMooreMachine(const TAWithCounter<BufferSize> &from, const Bounds &M, const std::size_t numOfVariables, MooreMachine<BufferSize, std::pair<unsigned char, double>, DRTAState> &to) 
{
  // S_{old} -> Int -> S_{new}
  // Q = S \times Z(X)
  boost::unordered_map<std::vector<std::pair<std::shared_ptr<TAState>, DBM::Tuple>>, std::shared_ptr<DRTAState>> toNewState;
  std::unordered_map<std::shared_ptr<DRTAState>, std::unordered_multimap<std::shared_ptr<TAState>, DBM>> toOldStates;
  std::vector<std::shared_ptr<DRTAState>> currStates;

  const auto addNewState = [&](const std::unordered_multimap<std::shared_ptr<TAState>, DBM> &s) -> std::shared_ptr<DRTAState> {
    auto newState = std::make_shared<DRTAState>(std::any_of(s.begin(), s.end(), [](std::pair<std::shared_ptr<TAState>, DBM> ps) {
          // isMatch
          return ps.first->isMatch;
        }));
    std::vector<std::pair<std::shared_ptr<TAState>, DBM::Tuple>> s_tuple;
    for (auto &p: s) {
      s_tuple.emplace_back(p.first, p.second.toTuple());
    }
    std::sort(s_tuple.begin(), s_tuple.end());
    toNewState[s_tuple] = newState;
    toOldStates[newState] = s;
    to.counter[newState] = std::accumulate(s.begin(), s.end(), 0, [&](const int x, const std::pair<std::shared_ptr<TAState>, DBM> ps) -> int {
        auto it = from.counter.find(ps.first);
        assert(it != from.counter.end());
        if (it->second == BufferSize) {
          return BufferSize;
        } else if (ps.first->isMatch) {
          return std::max<int>(x, it->second);
        } else {
          return x;
        }
      });
    to.states.push_back(newState);
    currStates.push_back(newState);
    return newState;
  };

  // add initial state
  std::unordered_multimap<std::shared_ptr<TAState>, DBM> initConfs;
  {
    DBM initZone = DBM::zero(numOfVariables + 1);
    initZone.M = M;
    for (const auto &s: from.initialStates) {
      initConfs.emplace(s, initZone);
    }
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
          std::vector<std::pair<std::shared_ptr<TAState>, DBM::Tuple>> s_tuple;
          for (auto &p: targetPair.first) {
            s_tuple.emplace_back(p.first, p.second.toTuple());
          }
          std::sort(s_tuple.begin(), s_tuple.end());
          s_tuple.erase(std::unique(s_tuple.begin(), s_tuple.end()), s_tuple.end());

          auto it = toNewState.find(s_tuple);
          if (it == toNewState.end()) {
            // When this state is new
            // construct the next DTRAState
            s->nextMap[nextPair.first].push_back({targetPair.second, addNewState(targetPair.first)});
          } else {
            // When this state is not new
            s->nextMap[nextPair.first].push_back({targetPair.second, it->second});
          }
        }
      }
    }
  }

}
