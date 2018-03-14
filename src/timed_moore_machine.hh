#pragma once
#include <boost/unordered_set.hpp>

// (setq flycheck-clang-language-standard "c++14")
#include "dbm.hh"
#include "moore_machine.hh"

void constructNexts(const std::shared_ptr<DRTAState> &s,
                    std::unordered_map<std::shared_ptr<DRTAState>, std::unordered_multimap<std::shared_ptr<TAState>, DBM>> &toOldStates, 
                    boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> &nexts) 
{
  nexts.clear();
  for (auto old_s: toOldStates[s]) {
    // one-clock determinisation
    const auto &currentZone = old_s.second;
    for (const auto &nextPair: old_s.first->nextMap) {
      nexts[nextPair.first].reserve(nexts[nextPair.first].size() + nextPair.second.size());
      for (const TATransition &transition: nextPair.second) {
        DBM nextZone = currentZone;
        nextZone.elapse();
        // TODO: Doing this by DBM operation may be faster
        for (const auto &delta : transition.guard) {
          switch (delta.odr) {
          case Constraint::Order::lt:
            nextZone.tighten(delta.x,-1,{delta.c, false});
            break;
          case Constraint::Order::le:
            nextZone.tighten(delta.x,-1,{delta.c, true});
            break;
          case Constraint::Order::gt:
            nextZone.tighten(-1,delta.x,{-delta.c, false});
            break;
          case Constraint::Order::ge:
            nextZone.tighten(-1,delta.x,{-delta.c, true});
            break;
          }
        }
        if (nextZone.isSatisfiable()) {
          for (auto x: transition.resetVars) {
            nextZone.reset(x);
          }
          DBM constraintZone = DBM::zero(currentZone.getNumOfVar() + 1);
          constraintsToDBM(transition.guard, constraintZone);
          Bounds lowerBound, upperBound;
          currentZone.findDuration(constraintZone, lowerBound, upperBound);
          nexts[nextPair.first].emplace_back(transition.target.lock(), nextZone, lowerBound, upperBound);
        }
      }
    }
  }

}

void linearlizeNexts(const boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> &nexts,
                     const std::vector<std::pair<std::shared_ptr<TAState>, DBM>> &initConfs,
                     std::unordered_map<unsigned char, std::vector<std::pair<std::unordered_multimap<std::shared_ptr<TAState>, DBM>, Bounds>>> &nextLineared) 
{
  boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> nextsLower = nexts;
  boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> nextsUpper = nexts;

  for (auto& n: nextsLower) {
    std::sort(n.second.begin(), n.second.end(), [](const std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds> &x,
                                                   const std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds> &y) {
                return std::get<2>(x) > std::get<2>(y);
              });
    for (auto &l: n.second) {
      std::get<2>(l).first = -std::get<2>(l).first;
    }
  }
  for (auto& n: nextsUpper) {
    std::sort(n.second.begin(), n.second.end(), [](const std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds> &x,
                                                   const std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds> &y) {
                return std::get<3>(x) < std::get<3>(y);
              });
  }
  for (const auto &lower: nextsLower) {
    const unsigned char c = lower.first;
    std::deque<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>> lowerVec(lower.second.begin(), lower.second.end());
    std::deque<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>> upperVec(nextsUpper[c].begin(), nextsUpper[c].end());

    std::unordered_multimap<std::shared_ptr<TAState>, DBM> currentStates = {initConfs.begin(), initConfs.end()};
    const auto useLowerFront = [&]() {
      const Bounds addedBounds = std::get<2>(lowerVec.front());
      while (addedBounds == std::get<2>(lowerVec.front())) {
        const auto addedPair = std::make_pair(std::get<0>(lowerVec.front()), std::get<1>(lowerVec.front()));
        if (std::find(initConfs.begin(), initConfs.end(), addedPair) == initConfs.end()) {
          currentStates.emplace(std::move(addedPair));
        }
        lowerVec.pop_front();
      }
    };
    const auto useUpperFront = [&]() {
      const Bounds removedBounds = std::get<3>(upperVec.front());
      while (removedBounds == std::get<3>(upperVec.front())) {
        std::pair<std::shared_ptr<TAState>, DBM> removedPair = {std::get<0>(upperVec.front()), std::get<1>(upperVec.front())};
        if (std::find(initConfs.begin(), initConfs.end(), removedPair) == initConfs.end()) {
          auto its = currentStates.equal_range(std::get<0>(upperVec.front()));
          for (auto it = its.first; it != its.second;) {
            if (it->second == std::get<1>(upperVec.front())) {
              it = currentStates.erase(it);
              break;
            } else {
              ++it;
            }
          }
        }
        upperVec.pop_front();
      }
    };
    while (!lowerVec.empty() && std::get<2>(lowerVec.front()) <= Bounds{0, true}) {
      useLowerFront();
    }
    while (!lowerVec.empty()) {
      if (std::get<2>(lowerVec.front()).first < std::get<3>(upperVec.front()).first) {
        auto b = std::get<2>(lowerVec.front());
        b.second = !b.second;
        nextLineared[c].emplace_back(currentStates, b);
        useLowerFront();
      } else if (std::get<2>(lowerVec.front()).first > std::get<3>(upperVec.front()).first) {
        nextLineared[c].emplace_back(currentStates, std::get<3>(upperVec.front()));
        useUpperFront();
      } else if (std::get<2>(lowerVec.front()).second != std::get<3>(upperVec.front()).second) {
        nextLineared[c].emplace_back(currentStates, std::get<3>(upperVec.front()));
        useUpperFront();
        useLowerFront();
      } else if (std::get<2>(lowerVec.front()) == std::get<3>(upperVec.front())) {
        Bounds open = std::get<2>(lowerVec.front());
        open.second = false;
        Bounds closed = std::get<2>(lowerVec.front());
        closed.second = true;
        nextLineared[c].emplace_back(currentStates, open);
        if (std::get<2>(lowerVec.front()).second) {
          useLowerFront();
          nextLineared[c].emplace_back(currentStates, closed);
          useUpperFront();
        } else {
          useUpperFront();
          nextLineared[c].emplace_back(currentStates, closed);
          useLowerFront();
        }
      }
    }

    while (!upperVec.empty()) {
      nextLineared[c].emplace_back(currentStates, std::get<3>(upperVec.front()));
      useUpperFront();
    }
  }
}

// TAWithCounter -> TimedMooreMachine
template<int BufferSize>
void toTimedMooreMachine(TAWithCounter<BufferSize> &from, Bounds M, std::size_t numOfVariables, MooreMachine<BufferSize, unsigned char, DRTAState> &to) 
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
