#ifndef TPOT_TPOTDEFS_H
#define TPOT_TPOTDEFS_H

#include <array>
#include <string>

namespace TpotDefs
{
  /// total number of phi detectors  
  static constexpr int n_detectors_phi = 8;

  /// total number of phi detectors  
  static constexpr int n_detectors_z = 8;

  /// number of resist sector in each detector
  static constexpr int n_resist = 4;

  /// total number of detectors  
  static constexpr int n_detectors = 16;

  // detector names (ordered by tile_id (0 to 8) and layer (P or Z)
  const std::array<std::string, n_detectors> detector_names =
  {
    "M5P",  "M5Z",
    "M8P",  "M8Z",
    "M4P",  "M4Z",
    "M10P", "M10Z",
    "M9P",  "M9Z",
    "M2P",  "M2Z",
    "M6P",  "M6Z",
    "M7P",  "M7Z"
  };
  
}  
#endif
