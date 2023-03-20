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

/// \file GBTWord.h
/// \brief Classes for creation/interpretation of MVTX GBT data

#ifndef _MVTX_GBTWORD_H_
#define _MVTX_GBTWORD_H_

#include <cstdint>
#include <string>

namespace mvtx
{
constexpr uint64_t LANESMask = (0x1 << 9) - 1; // at most 9 lanes
/// GBT payload header flag
constexpr uint8_t GBTFlagDataHeader = 0xe0;
/// GBT payload trailer flag
constexpr uint8_t GBTFlagDataTrailer = 0xf0;
/// GBT trigger status word flag
constexpr uint8_t GBTFlagTrigger = 0xe8;
/// GBT diagnostic status word flag
constexpr uint8_t GBTFlagDiagnostic = 0xe4;
/// GBT calibration status word flag
constexpr uint8_t GBTFlagCalibration = 0xf8;

// GBT header flag in the RDH
constexpr uint8_t GBTFlagRDH = 0x00;
// GBT header flag for the ITS IB: 001 bbbbb with bbbbb -> Lane Number (0-8)
constexpr uint8_t GBTFlagDataIB = 0x20;
// GBT header flag for the ITS IB idagnostic : 101 bbbbb with bbbbb -> Lane Number (0-8)
constexpr uint8_t GBTFlagDiagnosticIB = 0xa0;

// GBT header flag for the ITS Status word : 111 bbbbb with bbbbb -> Lane Number
constexpr uint8_t GBTFlagStatus = 0xe0;

constexpr int GBTWordLength = 10;       // lentgh in bytes

struct GBTWord {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
  /// GBT word of 80 bits, bits 72:79 are reserver for GBT Header flag, the rest depends on specifications
  union {
    // RS: packing will be needed only if some of the members cross 64 bit boundary
    struct /*__attribute__((packed))*/ {
      uint64_t activeLanes : 28; /// 0:27   Bit map of lanes active and eligible for readout
      uint64_t na0hn : 36;       /// 28:71  reserved
      uint64_t na1hn : 8;        /// 28:71  reserved
      uint64_t id : 8;           /// 72:79  0xe0; Header Status Word (HSW) identifier
    };                           // HEADER NEW

    struct /*__attribute__((packed))*/ {
      uint64_t lanesStatus : 56;        /// 0:55  Bit map of “Valid Lane stops received”, 1 bit per lane, NOT USED
      uint64_t na0t : 8;                /// 56:63 reserved
      uint64_t packetDone : 1;          /// 64  = 1 when current trigger packets transmission done
      uint64_t transmissionTimeout : 1; /// 65  = 1 if timeout while waiting for data on lanes
      uint64_t na1t : 1;                /// 66  reserved
      uint64_t laneStartsViolation : 1; /// 67  = 1 if at least 1 lane (eligible for readout) had a “start violation”
      uint64_t na2t : 4;                /// 68:71  reserved
      //  uint8_t  id : 8;              /// = 0xf0; Trigger Data Trailer (TDT) identifier
    }; // TRAILER

    struct __attribute__((packed)) {
      uint64_t triggerType : 12; /// 0:11   12 lowest bits of trigger type received from CTP
      uint64_t internal : 1;     /// 12     Used in Continuous Mode for internally generated trigger
      uint64_t noData : 1;       /// 13     No data expected (too close to previous trigger or error)
      uint64_t continuation : 1; /// 14     following data is continuation of the trigger from the previous CRU page
      uint64_t na1tr : 1;        /// 15     reserved
      uint64_t bc : 12;          /// 16:27  HB or internal trigger BC count or trigger BC from CTP
      uint64_t na2tr : 4;        /// 28:31  reserved
      uint64_t orbit : 40;       /// 32:63  Last received HB Orbit or trigger orbit count/ Orbit as received from CTP
      //  uint8_t  id : 8;       /// = 0xe8; Trigger Data Header (TDH) identifier
    }; // TRIGGER

    struct __attribute__((packed)) {
      uint64_t calibUserField : 48; /// 0:47   user field
      uint64_t calibCounter : 24;   /// 48:71  elf-incrementing counter of
      //  uint64_t id : 8;            /// 72:79  0xf8; Calibration Status Word (HSW) identifier
    }; /// Calibration Data Word

    struct /*__attribute__((packed))*/ {
      uint64_t diagnosticData : 64; /// 0:63   Error specific diagnostic data
      uint64_t laneErrorID : 8;     /// 64:71  Identifier of the specific error condition
      //      uint64_t id : 8;           /// 72:79  0xe0;  Status Word (HSW) identifier
    }; // HEADER Legacy

    uint8_t data8[GBTWordLength]; // 80 bits GBT word + optional padding to 128 bits
  };
#pragma GCC diagnostic pop

  GBTWord() = default;

  /// check if the GBT Header corresponds to GBT payload header
  bool isDataHeader() const { return id == GBTFlagDataHeader; }

  /// check if the GBT Header corresponds to GBT payload trailer
  bool isDataTrailer() const { return id == GBTFlagDataTrailer; }

  /// check if the GBT Header corresponds to GBT trigger word
  bool isTriggerWord() const { return id == GBTFlagTrigger; }

  /// check if the GBT Header corresponds to Diagnostic data
  bool isDiagnosticWord() const { return id == GBTFlagDiagnostic; }

  /// check if the GBT Header corresponds to Calibration word
  bool isCalibrationWord() const { return id == GBTFlagCalibration; }

  /// check if the GBT Header corresponds to ITS IB data (header is combined with lanes info)
  bool isData() const { return (id & 0xe0) == GBTFlagDataIB; }

  /// check if the GBT Header corresponds to ITS IB diagnostics data (header is combined with lanes info)
  bool isCableDiagnostic() const { return (id & 0xe0) == GBTFlagDiagnosticIB; }

  /// check if the GBT Header corresponds to ITS IB or OB data (header is combined with lanes/connector info)

  bool isStatus() const { return (id & 0xe0) == GBTFlagStatus; }

  const uint8_t* getW8() const { return data8; }

  uint8_t getHeader() const { return id; }

  void printX() const;

  std::string asString() const;

//  ClassDefNV(GBTWord, 1);
};


struct GBTDataHeader : public GBTWord {
  /// Definition of ITS/MFT GBT Header: 80 bits long word
  /// In CRU data it must be the 1st word of the payload
  ///
  /// bits  0 : 27, Active lanes pattern
  /// bits 28 : 71, not used
  /// bits 72 : 79, header/trailer indicator

  GBTDataHeader() { id = GBTFlagDataHeader; }
  GBTDataHeader(uint32_t lanes)
  {
    id = GBTFlagDataHeader;
    activeLanes = lanes;
  }
//  ClassDefNV(GBTDataHeader, 1);
};

struct GBTDataTrailer : public GBTWord {
  /// Definition of MVTX GBT trailer: 80 bits long word
  /// In CRU data it must be the last word of the payload
  ///
  /// bits  0 : 27, Lanes stops received   // not used at the moment
  /// bits 28 : 55, Lane timeouts received // not used at the moment
  /// bits 56 : 63, not used
  /// bits 64 : 71, State of GBT_Packet:
  ///               4: lane_timeouts,  if at least 1 lane timed out
  ///               3: lane_starts_violation,  if at least 1 lane had a start violation
  ///               2: packet_overflow, if max number of packets reached
  ///               1: transmission_timeout, if timeout of transmission (lanes)
  ///               0: packet_done, if Packet finished
  enum PacketStates : int {
    PacketDone = 0,                              // Packet finished
    TransmissionTimeout = 1,                     // if timeout of transmission (lanes)
    PacketOverflow = 2,                          // if max number of packets reached
    LaneStartsViolation = 3,                     // if at least 1 lane had a start violation
    LaneTimeouts = 4,                            // if at least 1 lane timed out
    NStatesDefined,                              // total number of states defined
    MaxStateCombinations = 0x1 << NStatesDefined // number of possible combinations
  };

  GBTDataTrailer() { id = GBTFlagDataTrailer; }
 /* GBTDataTrailer(int lanesStop, int lanesTimeOut, bool done, bool transmTO, bool overflow, bool laneStViol, bool laneTO)
  {
    lanesStops = lanesStop;
    lanesTimeout = lanesTimeOut;
    packetDone = done;
    transmissionTimeout = transmTO;
    packetOverflow = overflow;
    laneStartsViolation = laneStViol;
    laneTimeouts = laneTO;
    id = GBTFlagDataTrailer;
  }
*/
  uint8_t getPacketState() const { return data8[8]; }

  void setByte(uint8_t v, int which) = delete;

//  ClassDefNV(GBTDataTrailer, 1);
};

struct GBTTrigger : public GBTWord {
  /// Definition of ITS/MFT Trigger status word
  /// Precedes the single trigger (continuous or triggered) data block
  ///
  /// bits  0 : 11, Trigger Type, 12 lowest bits of trigger type received from CTP for HB or 0
  /// bit  12       Internal Trigger, Used in Continuous Mode for internally generated trigger
  /// bits 13       No data expected (too close to previous trigger or error)
  /// bits 14       reserved
  /// bits 15       reserved
  /// bits 16:27    Trigger BC, HB or internal trigger BC count
  /// bits 28:31    reserved
  /// bits 32:63    Trigger Orbit, Last received HB Orbit + internal trigger orbit count
  /// bits 64:71    reserved
  /// bits 72:79    ID = 0xc0; Trigger Status Word (TrgSW) identifier

  GBTTrigger() { id = GBTFlagTrigger; }
  void setByte(uint8_t v, int which) = delete;

//  ClassDefNV(GBTTrigger, 1);
};

struct GBTData : public GBTWord {
  /// Definition of ITS/MFT GBT paiload: 80 bits long word (can be padded to 128b) shipping
  /// Alpide data and having GBTHeader at positions 72:79
 public:
  /// extract lane of the IB as lowest 5 bits (ATTENTION: no check if this is really an IB header)
  static int getLaneIB(uint8_t v) { return v & 0x1f; }
  static int getCableID(uint8_t v) { return v & 0x1f; } // combined connector and lane

  /// extract lane of the IB as lowest 5 bits (ATTENTION: no check if this is really an IB header)
  int getLaneIB() const { return getLaneIB(id); }
  int getCableID() const { return getCableID(id); }    // combined connector and lane

//  ClassDefNV(GBTData, 1);
};

struct GBTDiagnostic : public GBTWord {
  /// Definition of GBT diagnostic word
  /// In CRU data it must be the only word after the RDH with stop
  ///
  /// bits  0 : 71, reserved
  /// bits 72 : 79, diagnostic flag

  GBTDiagnostic() { id = GBTFlagDiagnostic; }
//  ClassDefNV(GBTDiagnostic, 1);
};

struct GBTCableDiagnostic : public GBTWord {
  /// Definition of cable diagnostic word
  ///
  /// bits  0 : 64, Error specific diagnostic data
  /// bits 63 : 71, Identifier of the specific error condition
  /// bits 72 : 79, IB or OB diagnostic flag + cable id

  GBTCableDiagnostic(int lane = 0) { id = (GBTFlagDiagnosticIB) | (lane & 0x1f); }
  int getCableID() const { return id & 0x1f; } // combined connector and lane
  bool isIB() const { return (id & 0xe0) == GBTFlagDiagnosticIB; }
//  ClassDefNV(GBTCableDiagnostic, 1);
};

struct GBTCableStatus : public GBTWord { // not sure this is correct, FIXME
  /// Definition of cable status word
  ///
  /// bits 72 : 79, Status flag + cable id

  GBTCableStatus(int lane = 0) { id = GBTFlagStatus | (lane & 0x1f); }
  int getCableID() const { return id & 0x1f; } // combined connector and lane
//  ClassDefNV(GBTCableStatus, 1);
};

struct GBTCalibration : public GBTWord { // calibration data word
  /// bits  0 : 47, user-written tagging fields
  /// bits 48 : 71, self-incrementing counter of CDW words
  /// bits 72 : 79, calibration indicator

  GBTCalibration() { id = GBTFlagCalibration; }
  GBTCalibration(uint64_t userData, uint16_t counter = 0)
  {
    id = GBTFlagCalibration;
    calibUserField = userData & ((0x1UL << 48) - 1);
    calibCounter = counter & ((0x1 << 24) - 1);
  }
//  ClassDefNV(GBTCalibration, 1);
};
} // namespace mvtx

#endif
