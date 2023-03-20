// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \brief  Interaction record encoding BC, orbit, time

#ifndef _MVTX_INTERACTIONRECORD_H_
#define _MVTX_INTERACTIONRECORD_H_

#include <cmath>
#include <cstdint>
#include <functional>
#include <iosfwd>

namespace mvtx
{
namespace lhcConstants
{
constexpr int LHCMaxBunches = 3564;                              // max N bunches
constexpr double LHCRFFreq = 400.789e6;                          // LHC RF frequency in Hz
constexpr double LHCBunchSpacingNS = 10 * 1.e9 / LHCRFFreq;      // bunch spacing in ns (10 RFbuckets)
constexpr double LHCOrbitNS = LHCMaxBunches * LHCBunchSpacingNS; // orbit duration in ns
constexpr double LHCRevFreq = 1.e9 / LHCOrbitNS;                 // revolution frequency
constexpr double LHCBunchSpacingMUS = LHCBunchSpacingNS * 1e-3;  // bunch spacing in \mus (10 RFbuckets)
constexpr double LHCOrbitMUS = LHCOrbitNS * 1e-3;                // orbit duration in \mus
} // namespace lhcConstants

//!< TODO: Add RHIC constants

struct InteractionRecord {
  // information about bunch crossing and orbit
  static constexpr uint16_t DummyBC = 0xffff;
  static constexpr uint64_t DummyOrbit = 0xffffffffff;
  static constexpr double DummyTime = DummyBC * mvtx::lhcConstants::LHCBunchSpacingNS + DummyOrbit * mvtx::lhcConstants::LHCOrbitNS;

  uint16_t bc = DummyBC;       ///< bunch crossing ID of interaction
  uint64_t orbit = DummyOrbit; ///< LHC orbit

  InteractionRecord() = default;

  InteractionRecord(double tNS)
  {
    setFromNS(tNS);
  }

  InteractionRecord(uint16_t b, uint64_t orb) : bc(b), orbit(orb){};

  InteractionRecord(const InteractionRecord& src) = default;

  InteractionRecord& operator=(const InteractionRecord& src) = default;

  void clear()
  {
    bc = DummyBC;
    orbit = DummyOrbit;
  }

  bool isDummy() const
  {
    return bc > mvtx::lhcConstants::LHCMaxBunches;
  }

  void setFromNS(double ns)
  {
    bc = ns2bc(ns, orbit);
  }

  static double bc2ns(int bc, uint64_t orbit)
  {
    return bc * mvtx::lhcConstants::LHCBunchSpacingNS + orbit * mvtx::lhcConstants::LHCOrbitNS;
  }

  static int ns2bc(double ns, uint64_t& orb)
  {
    orb = ns > 0 ? ns / mvtx::lhcConstants::LHCOrbitNS : 0;
    ns -= orb * mvtx::lhcConstants::LHCOrbitNS;
    return std::round(ns / mvtx::lhcConstants::LHCBunchSpacingNS);
  }

  double bc2ns() const
  {
    return bc2ns(bc, orbit);
  }

  bool operator==(const InteractionRecord& other) const
  {
    return (bc == other.bc) && (orbit == other.orbit);
  }

  bool operator!=(const InteractionRecord& other) const
  {
    return (bc != other.bc) || (orbit != other.orbit);
  }

  int64_t differenceInBC(const InteractionRecord& other) const
  {
    // return difference in bunch-crossings
    int64_t diffBC = int(bc) - other.bc;
    if (orbit != other.orbit) {
      diffBC += (int64_t(orbit) - other.orbit) * mvtx::lhcConstants::LHCMaxBunches;
    }
    return diffBC;
  }

  float differenceInBCNS(const InteractionRecord& other) const
  {
    // return difference in bunch-crossings in ns
    return differenceInBC(other) * mvtx::lhcConstants::LHCBunchSpacingNS;
  }

  float differenceInBCMS(const InteractionRecord& other) const
  {
    // return difference in bunch-crossings in ms
    return differenceInBC(other) * mvtx::lhcConstants::LHCBunchSpacingMUS;
  }

  int64_t toLong() const
  {
    // return as single long number
    return (int64_t(orbit) * mvtx::lhcConstants::LHCMaxBunches) + bc;
  }

  void setFromLong(int64_t l)
  {
    // set from long BC counter
    bc = l % mvtx::lhcConstants::LHCMaxBunches;
    orbit = l / mvtx::lhcConstants::LHCMaxBunches;
  }

  static InteractionRecord long2IR(int64_t l)
  {
    // set from long BC counter
    return {uint16_t(l % mvtx::lhcConstants::LHCMaxBunches), uint32_t(l / mvtx::lhcConstants::LHCMaxBunches)};
  }

  bool operator>(const InteractionRecord& other) const
  {
    return (orbit == other.orbit) ? (bc > other.bc) : (orbit > other.orbit);
  }

  bool operator>=(const InteractionRecord& other) const
  {
    return !((*this) < other);
  }

  bool operator<(const InteractionRecord& other) const
  {
    return (orbit == other.orbit) ? (bc < other.bc) : (orbit < other.orbit);
  }

  bool operator<=(const InteractionRecord& other) const
  {
    return !((*this) > other);
  }

  InteractionRecord operator--()
  {
    // prefix decrement operator, no check for orbit wrap
    if (!bc--) {
      orbit--;
      bc = mvtx::lhcConstants::LHCMaxBunches - 1;
    }
    return InteractionRecord(*this);
  }

  InteractionRecord operator--(int)
  {
    // postfix decrement operator, no check for orbit wrap
    InteractionRecord tmp(*this);
    if (!bc--) {
      orbit--;
      bc = mvtx::lhcConstants::LHCMaxBunches - 1;
    }
    return tmp;
  }

  InteractionRecord operator++()
  {
    // prefix increment operator,no check for orbit wrap
    if ((++bc) == mvtx::lhcConstants::LHCMaxBunches) {
      orbit++;
      bc = 0;
    }
    return InteractionRecord(*this);
  }

  InteractionRecord operator++(int)
  {
    // postfix increment operator, no check for orbit wrap
    InteractionRecord tmp(*this);
    if ((++bc) == mvtx::lhcConstants::LHCMaxBunches) {
      orbit++;
      bc = 0;
    }
    return tmp;
  }

  InteractionRecord& operator+=(int64_t dbc)
  {
    // bc self-addition operator, no check for orbit wrap
    auto l = toLong() + dbc;
    bc = l % mvtx::lhcConstants::LHCMaxBunches;
    orbit = l / mvtx::lhcConstants::LHCMaxBunches;
    return *this;
  }

  InteractionRecord& operator-=(int64_t dbc)
  {
    // bc self-subtraction operator, no check for orbit wrap
    return operator+=(-dbc);
  }

  InteractionRecord& operator+=(const InteractionRecord& add)
  {
    // InteractionRecord self-addition operator, no check for orbit wrap
    auto l = this->toLong() + add.toLong();
    bc = l % mvtx::lhcConstants::LHCMaxBunches;
    orbit = l / mvtx::lhcConstants::LHCMaxBunches;
    return *this;
  }

  InteractionRecord& operator-=(const InteractionRecord& add)
  {
    // InteractionRecord self-subtraction operator, no check for orbit wrap
    auto l = this->toLong() - add.toLong();
    bc = l % mvtx::lhcConstants::LHCMaxBunches;
    orbit = l / mvtx::lhcConstants::LHCMaxBunches;
    return *this;
  }

  InteractionRecord operator+(int64_t dbc) const
  {
    // bc addition operator, no check for orbit wrap
    auto l = toLong() + dbc;
    return InteractionRecord(l % mvtx::lhcConstants::LHCMaxBunches, l / mvtx::lhcConstants::LHCMaxBunches);
  }

  InteractionRecord operator-(int64_t dbc) const
  {
    // bc subtraction operator, no check for orbit wrap
    auto l = toLong() - dbc;
    return InteractionRecord(l % mvtx::lhcConstants::LHCMaxBunches, l / mvtx::lhcConstants::LHCMaxBunches);
  }

  InteractionRecord operator+(const InteractionRecord& add) const
  {
    // InteractionRecord addition operator, no check for orbit wrap
    auto l = this->toLong() + add.toLong();
    return InteractionRecord(l % mvtx::lhcConstants::LHCMaxBunches, l / mvtx::lhcConstants::LHCMaxBunches);
  }

  InteractionRecord operator-(const InteractionRecord& add) const
  {
    // InteractionRecord subtraction operator, no check for orbit wrap
    auto l = this->toLong() - add.toLong();
    return InteractionRecord(l % mvtx::lhcConstants::LHCMaxBunches, l / mvtx::lhcConstants::LHCMaxBunches);
  }

  void print() const;
  std::string asString() const;
  friend std::ostream& operator<<(std::ostream& stream, InteractionRecord const& ir);

// ClassDefNV(InteractionRecord, 3);
};


struct InteractionTimeRecord : public InteractionRecord {
  double timeInBCNS = 0.; ///< time in NANOSECONDS relative to orbit/bc

  InteractionTimeRecord() = default;

  /// create from the interaction record and time in the bunch (in ns)
  InteractionTimeRecord(const InteractionRecord& ir, double t_in_bc) : InteractionRecord(ir), timeInBCNS(t_in_bc)
  {
  }

  /// create from the abs. (since orbit=0/bc=0) time in NS
  InteractionTimeRecord(double tNS) : InteractionRecord(tNS)
  {
    timeInBCNS = tNS - bc2ns();
  }

  /// set the from the abs. (since orbit=0/bc=0) time in NS
  void setFromNS(double tNS)
  {
    InteractionRecord::setFromNS(tNS);
    timeInBCNS = tNS - bc2ns();
  }

  void clear()
  {
    InteractionRecord::clear();
    timeInBCNS = 0.;
  }

  double getTimeOffsetWrtBC() const
  {
    return timeInBCNS;
  }

  /// get time in ns from orbit=0/bc=0
  double getTimeNS() const
  {
    return timeInBCNS + bc2ns();
  }

  bool operator==(const InteractionTimeRecord& other) const
  {
    return this->InteractionRecord::operator==(other) && (timeInBCNS == other.timeInBCNS);
  }

  bool operator!=(const InteractionTimeRecord& other) const
  {
    return this->InteractionRecord::operator!=(other) || (timeInBCNS != other.timeInBCNS);
  }

  bool operator>(const InteractionTimeRecord& other) const
  {
    return (this->InteractionRecord::operator>(other)) || (this->InteractionRecord::operator==(other) && (timeInBCNS > other.timeInBCNS));
  }

  bool operator>=(const InteractionTimeRecord& other) const
  {
    return !((*this) < other);
  }

  bool operator<(const InteractionTimeRecord& other) const
  {
    return (this->InteractionRecord::operator<(other)) || (this->InteractionRecord::operator==(other) && (timeInBCNS < other.timeInBCNS));
  }

  bool operator<=(const InteractionTimeRecord& other) const
  {
    return !((*this) > other);
  }

  void print() const;
  std::string asString() const;
  friend std::ostream& operator<<(std::ostream& stream, InteractionTimeRecord const& ir);

//  ClassDefNV(InteractionTimeRecord, 1);
};
} // namespace mvtx

namespace std
{
// defining std::hash for InteractionRecord to be used with std containers
template <>
struct hash<mvtx::InteractionRecord> {
 public:
  size_t operator()(const mvtx::InteractionRecord& ir) const
  {
    return ir.toLong();
  }
};
} // namespace std

#endif
