/**
 * Project specific RTE interfaces.
 */

#ifndef IFCRTEPRJ_H__
#define IFCRTEPRJ_H__

#include <IfcRte.h>
#include <stdint.h>

// -----------------------------------------------------
/// Defines
// -----------------------------------------------------
#define NR_SIGNAL_PICTURES    4  // hp0, hp1, hp2, hp3
#define NR_CLASSIFIERS    5
#define NR_SIGNALS        5
#define NR_CLASSES        4
#define NR_SIGNAL_LED     5
/// Cycle time of ramp calculations
#define CFG_SPEED_CYLCE_TIME  10
#define CFG_NR_ONBOARD_OUTPUT 50
#define CFG_NR_EXTERNAL_OUTPUT  (5 * 8)

// -----------------------------------------------------
/// Types
// -----------------------------------------------------

/// [1/32768 % / 10 ms]
template<int CycleTime>
class Speed16
{
public:
  /// [1/32768 % / 10 ms]
  uint16_t unSpeed;
  ///Convert unTime [%] into Speed16 value
  constexpr static uint16_t ConvTime(uint16_t unTime) { return (uint16_t) (((uint16_t) 32768u * (uint16_t) CycleTime) / unTime); }
};

/// 0: 0%, 0x8000: 100%
class Percent16
{
public:
  uint16_t unPercent;
  /// 32768u / 100u ~ 41943u / 128u
  constexpr static Percent16 ConvPercent100(uint8_t u8Percent) { return uint32_t{ u8Percent } * 41943u / 128u; }
};

/// Array of type T with N elements
template<class T, size_t N>
class Array
{
protected:
  T aValues[N];

public:
  typedef Array<T, N> This;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef pointer iterator;
  typedef const_pointer const_iterator;

  Array() {}
  Array(const T aInit[N]) 
  {
    size_t i;
    for (i = 0u; i < N; i++)
    {
      aValues[i] = aInit[i];
    }
  }
  constexpr size_t size() const noexcept { return N; }
  static constexpr size_t Size() const noexcept { return N; }
  T& operator[](size_t n) { return aValues[i]; }
  const T& operator[](size_t n) const { return aValues[i]; }
  This& operator=(const This& toCopy)
  {
    size_t i;
    for (i = 0; i < N; i++)
    {
      aValues[i] = toCopy.aValues[i];
    }
    return *this;
  }
  iterator begin() { return &aValues[0]; }
  const_iterator begin() const { return &aValues[0]; }
  iterator end() { return &aValues[N]; }
  const_iterator end() const { return &aValues[N]; }
};

/// Array of uint8_t for classified integral values
typedef Array<uint8_t, NR_CLASSIFIERS> tClassifiedValueArray;

/// Output of a single signal
typedef Array<uint8_t, NR_SIGNAL_LED> tSignalTgtIntArray;

/// Speed information based on cyclic updates of CFG_SPEED_CYLCE_TIME
typedef Speed16<CFG_SPEED_CYLCE_TIME> tSpeed16;

/// Output of signals
typedef struct
{
  tSignalTgtIntArray aIntArray;
  tSpeed16 unSpeedUp;
  tSpeed16 unSpeedDn;
} tSignalTgt;

/// Ouptut of multiple signals
typedef Array<tSignalTgt, NR_SIGNALS> tSignalTgtArray;

typedef Array<Percent16, CFG_NR_ONBOARD_OUTPUT> tOnboardTargetArray;
typedef Array<Percent16, CFG_NR_EXTERNAL_OUTPUT> tExternalTargetArray;

typedef Array<uint8_t, NR_CLASSIFIERS> tClassifierMapArray;
typedef Array<tSignalTgt, NR_SIGNAL_PICTURES> tSignalPicturesArray;
/// Configuration data for signals
typedef struct
{
  tSpeed16 unSpeedUp;
  tSpeed16 unSpeedDn;
  tClassifierMapArray aClassifierMap;
  tSignalPicturesArray aSignalPictures;
} tRailwaySignalConfig;

// -----------------------------------------------------
/// Coding data
// -----------------------------------------------------

/// Calibration values of a single LED output
typedef struct
{
  uint8_t u8Type;    ///< type of output: 0 = none, 1 = DO port or PWM port, 2 = Shift register, others: none
  uint8_t u8Output;  ///< depends on u8Type:
                     ///< u8Type == 1: specifies onboard pin (DO or PWM)
                     ///< u8Type == 2: specifies position in shift register output
  /// Constants for u8Type
  typedef enum
  {
    NONE = 0,
    DO_PWM_PORT = 1,
    SHIFT_REG_PORT = 2
  } tTypeEnum;
} tCalLedR;

/// Mapping for a single signal
typedef Array<tCalLedR, NR_SIGNAL_LED> tCalLedRSignal;
/// The configuration for all signals
typedef Array<tCalLedRSignal, NR_SIGNALS> tCalLedRSignalArray;

// -----------------------------------------------------
/// Interfaces
// -----------------------------------------------------

/// RTE interface for classified input values
typedef IfcRteSR<tClassifiedValueArray> IfcRteSRClassifiedValueArray;

/// RTE interface for signal target values
typedef IfcRteSR<tSignalTgtArray> IfcRteSRSignalTgtArray;

/// RTE interface for onboard target values
typedef IfcRteSR<tOnboardTargetArray> IfcRteSROnboardTarget;

/// RTE interface for target values of external switches
typedef IfcRteSR<tExternalTargetArray> IfcRteSRExternalTarget;

/// RTE interface for calibration values to map signal target values to onboard pins or shift registers
typedef IfcRteSRCal<tCalLedRSignalArray> IfcRteSRCalLedRSignalArray;

#endif // IFCRTEPRJ_H__
