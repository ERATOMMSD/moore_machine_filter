#pragma once
// (setq flycheck-clang-language-standard "c++14")
#include <cmath>
#include <limits>
#include <algorithm>
#include <boost/unordered_map.hpp>

#include "constraint.hh"

using Bounds = std::pair<double, bool>;
static inline Bounds
operator+ (const Bounds &a,const Bounds &b) {
  return Bounds(a.first + b.first, a.second && b.second);
}
static inline Bounds
operator- (const Bounds &a,const Bounds &b) {
  return Bounds(a.first - b.first, a.second && b.second);
}
static inline void
operator+= (Bounds &a, const Bounds b) {
  a.first += b.first;
  a.second = a.second && b.second;
}
static inline std::ostream& operator << (std::ostream& os, const Bounds& b) {
  os << "(" << b.first << ", " << b.second << ")";
  return os;
}

#include <eigen3/Eigen/Core>
//! @TODO configure include directory for eigen

struct DBM {
  using Tuple = std::tuple<std::vector<Bounds>,Bounds>;
  Eigen::Matrix<Bounds, Eigen::Dynamic, Eigen::Dynamic> value;
  Bounds M;

  inline std::size_t getNumOfVar() const {
    return value.cols() - 1;
  }

  inline void cutVars (std::shared_ptr<DBM> &out,std::size_t from,std::size_t to) {
    out = std::make_shared<DBM>();
    out->value.resize(to - from + 2, to - from + 2);
    out->value.block(0,0,1,1) << Bounds(0,true);
    out->value.block(1, 1, to - from + 1, to - from + 1) = value.block(from + 1, from + 1, to - from + 1,to - from + 1);
    out->value.block(1, 0, to - from + 1, 1) = value.block(from + 1, 0, to - from + 1, 1);
    out->value.block(0, 1, 1, to - from + 1) = value.block(0, from + 1, 1, to - from + 1);
    out->M = M;
  }
  
  static DBM zero(int size) {
    static DBM zeroZone;
    if (zeroZone.value.cols() == size) {
      return zeroZone;
    }
    zeroZone.value.resize(size, size);
    zeroZone.value.fill(Bounds(0, true));
    return zeroZone;
  }

  std::tuple<std::vector<Bounds>,Bounds> toTuple() const {
    // omit (0,0)
    return std::tuple<std::vector<Bounds>,Bounds>(std::vector<Bounds>(value.data() + 1, value.data() + value.size()),M);
  }

  //! @brief add the constraint x - y \le (c,s)
  void tighten(ClockVariables x, ClockVariables y, Bounds c) {
    x++;
    y++;
    value(x,y) = std::min(value(x, y), c);
    close1(x);
    close1(y);
  }

  void close1(ClockVariables x) {
    for (int i = 0; i < value.rows(); i++) {
      value.row(i) = value.row(i).array().min(value.row(x).array() + value(i, x));
      //      for (int j = 0; j < value.cols(); j++) {
      //        value(i, j) = std::min(value(i, j), value(i, x) + value(x, j));
      //      }
    }
  }
  
  // The reset value is always (0, \le)
  void reset(ClockVariables x) {
    // 0 is the special varibale here
    x++;
    value(0,x) = Bounds(0, true);
    value(x,0) = Bounds(0, true);
    value.col(x).tail(value.rows() - 1) = value.col(0).tail(value.rows() - 1);
    value.row(x).tail(value.cols() - 1) = value.row(0).tail(value.cols() - 1);
  }
  
  void elapse() {
    static constexpr Bounds infinity = Bounds(std::numeric_limits<double>::infinity(), false);
    value.col(0).fill(Bounds(infinity));
    for (int i = 0; i < value.row(0).size(); ++i) {
      value.row(0)[i].second = false;
    }
  }

  void canonize() {
    for (int k = 0; k < value.cols(); k++) {
      close1(k);
    }
  }

  bool isSatisfiable() {
    canonize();
    return (value + value.transpose()).minCoeff() >= Bounds(0.0, true);
  }

  void abstractize() {
    static constexpr Bounds infinity = Bounds(std::numeric_limits<double>::infinity(), false);
    for (auto it = value.data(); it < value.data() + value.size(); it++) {
      if (*it >= M) {
        *it = Bounds(infinity);
      }
    }
  }

  void makeUnsat() {
    value(0, 0) = Bounds(-std::numeric_limits<double>::infinity(), false);
  }

  bool operator== (DBM z) const {
    z.value(0,0) = value(0,0);
    return value == z.value;
  }

  void operator&=(const DBM &z) {
    value.array() = value.array().min(z.value.array());
    canonize();
  }

  // find [lower, upper] = \{t \mid value + t \cap z \ne \emptyset\}
  void findDuration(const DBM &z, Bounds &lower, Bounds &upper) const {
#ifdef DEBUG
    assert(z.value.cols() == value.cols());
    assert(z.value.rows() == value.rows());
#endif
    DBM v = DBM::zero(value.cols() + 1);
    Eigen::Matrix<Bounds, Eigen::Dynamic, Eigen::Dynamic> &valueT = v.value;
    valueT << value, value.col(0), value.row(0), Bounds(0, true);
    valueT(valueT.cols() - 1, 0) = Bounds(std::numeric_limits<double>::infinity(), false);
    v.elapse();
    // take intersection
    {
      Eigen::Matrix<Bounds, Eigen::Dynamic, Eigen::Dynamic> x(valueT.cols(), valueT.cols());
      Eigen::Matrix<Bounds, Eigen::Dynamic, 1> xv(valueT.cols() - 1);
      xv.fill(Bounds(std::numeric_limits<double>::infinity(), false));
      x << z.value, xv, xv.transpose(), Bounds(0, true);
      x(0, x.cols() - 1) = Bounds(0, false);
      x(x.cols() - 1, x.cols() - 1) = Bounds(0, true);
      valueT.array() = valueT.array().min(x.array());
    }

    v.canonize();
    upper = valueT(valueT.cols() - 1, 0);
    lower = valueT(0, valueT.cols() - 1);
  }
};

/*!
  @brief Construct DBM from a vector of constraints 

  @note The size of the given DBM D must be set correctly.
 */
inline void constraintsToDBM (const std::vector<Constraint> &guard, DBM &D) {
  D.value.fill(Bounds(std::numeric_limits<double>::infinity(), false));
  D.value.diagonal().fill(Bounds(0, true));
  D.value.row(0).fill(Bounds(0, true));
  for (const auto &delta : guard) {
    switch (delta.odr) {
    case Constraint::Order::lt:
      D.tighten(delta.x,-1,{delta.c, false});
      break;
    case Constraint::Order::le:
      D.tighten(delta.x,-1,{delta.c, true});
      break;
    case Constraint::Order::gt:
      D.tighten(-1,delta.x,{-delta.c, false});
      break;
    case Constraint::Order::ge:
      D.tighten(-1,delta.x,{-delta.c, true});
      break;
    }
  }
}

// struct ZoneAutomaton : public AbstractionAutomaton<DBM> {
//   struct TAEdge {
//     State source;
//     State target;
//     Alphabet c;
//     std::vector<Alphabet> resetVars;
//     std::vector<Constraint> guard;
//   };

//   boost::unordered_map<std::tuple<State, State, Alphabet>, TAEdge> edgeMap;
//   boost::unordered_map<std::pair<TAState, typename DBM::Tuple>, RAState> zones_in_za;
//   int numOfVariables;
// };

// static inline std::ostream& operator << (std::ostream& os, const DBM& z) {
//   for (int i = 0; i < z.value.rows();i++) {
//     for (int j = 0; j < z.value.cols();j++) {
//       os << z.value(i,j);
//     }
//     os << "\n";
//   }
//   os << std::endl;
//   return os;
// }


