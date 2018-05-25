#include "timed_moore_machine.hh"

void constructNexts(DRTAState *s,
                    std::unordered_map<DRTAState*, std::unordered_multimap<TAState*, DBM>> &toOldStates, 
                    boost::unordered_map<unsigned char, std::vector<std::tuple<TAState*, DBM, Bounds, Bounds>>> &nexts) 
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
          nexts[nextPair.first].emplace_back(transition.target, nextZone, lowerBound, upperBound);
        }
      }
    }
  }
}

void linearlizeNexts(const boost::unordered_map<unsigned char, std::vector<std::tuple<TAState*, DBM, Bounds, Bounds>>> &nexts,
                     const std::unordered_multimap<TAState*, DBM> &initConfs,
                     std::unordered_map<unsigned char, std::vector<std::pair<std::unordered_multimap<TAState*, DBM>, Bounds>>> &nextLineared) 
{
  boost::unordered_map<unsigned char, std::vector<std::tuple<TAState*, DBM, Bounds, Bounds>>> nextsLower = nexts;
  boost::unordered_map<unsigned char, std::vector<std::tuple<TAState*, DBM, Bounds, Bounds>>> nextsUpper = nexts;

  for (auto& n: nextsLower) {
    std::sort(n.second.begin(), n.second.end(), [](const std::tuple<TAState*, DBM, Bounds, Bounds> &x,
                                                   const std::tuple<TAState*, DBM, Bounds, Bounds> &y) {
                return std::get<2>(x) > std::get<2>(y);
              });
    for (auto &l: n.second) {
      std::get<2>(l).first = -std::get<2>(l).first;
    }
  }
  for (auto& n: nextsUpper) {
    std::sort(n.second.begin(), n.second.end(), [](const std::tuple<TAState*, DBM, Bounds, Bounds> &x,
                                                   const std::tuple<TAState*, DBM, Bounds, Bounds> &y) {
                return std::get<3>(x) < std::get<3>(y);
              });
  }
  for (const auto &lower: nextsLower) {
    const unsigned char c = lower.first;
    std::deque<std::tuple<TAState*, DBM, Bounds, Bounds>> lowerVec(lower.second.begin(), lower.second.end());
    std::deque<std::tuple<TAState*, DBM, Bounds, Bounds>> upperVec(nextsUpper[c].begin(), nextsUpper[c].end());

    std::unordered_multimap<TAState*, DBM> currentStates = {initConfs.begin(), initConfs.end()};
    const auto useLowerFront = [&]() {
      const Bounds addedBounds = std::get<2>(lowerVec.front());
      while (!lowerVec.empty() && addedBounds == std::get<2>(lowerVec.front())) {
        const auto addedPair = std::make_pair(std::get<0>(lowerVec.front()), std::get<1>(lowerVec.front()));
        auto its = initConfs.equal_range(std::get<0>(lowerVec.front()));
        for (auto it = its.first; it != its.second;) {
          if (it->second == std::get<1>(lowerVec.front())) {
            return;
          } else {
            ++it;
          }
        }
        currentStates.emplace(std::move(addedPair));
        lowerVec.pop_front();
      }
    };
    const auto useUpperFront = [&]() {
      const Bounds removedBounds = std::get<3>(upperVec.front());
      while (!upperVec.empty() && removedBounds == std::get<3>(upperVec.front())) {
        std::pair<TAState*, DBM> removedPair = {std::get<0>(upperVec.front()), std::get<1>(upperVec.front())};
        {
          auto its = initConfs.equal_range(std::get<0>(upperVec.front()));
          for (auto it = its.first; it != its.second;) {
            if (it->second == std::get<1>(upperVec.front())) {
              return;
            } else {
              ++it;
            }
          }
        }
        {
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

    if (nextLineared[c].empty() || !std::isinf(nextLineared[c].back().second.first)) {
      nextLineared[c].emplace_back(initConfs, Bounds{std::numeric_limits<double>::infinity(), false});
    }
  }
}
