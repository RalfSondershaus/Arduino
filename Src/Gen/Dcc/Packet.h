/**
  * @file Packet.h
  *
  * @author Ralf Sondershaus
  *
  * Defines class Packet in namespace Dcc
  */

#ifndef DCC_PACKET_H
#define DCC_PACKET_H

#include <Std_Types.h>
#include <Util/Math.h>
#include <Util/Array.h>

namespace Dcc
{
  // -----------------------------------------------------
  /// A packet has 0 ... MaxBytes bytes.
  ///
  /// @param MaxBytes Maximal number of bytes in a packet
  // -----------------------------------------------------
  template<int MaxBytes = 6>
  class Packet
  {
  protected:
    /// array of bytes
    typedef Util::Array<uint8, MaxBytes> ByteArray;

    /// number of bits received
    uint16 unNrBits;
    /// byte array
    ByteArray bytes;

    /// Return current byte index (index into array of bytes)
    uint16 byteIdx() const noexcept { return static_cast<uint16>(unNrBits / 8u); }
    /// Return current bit index (in current byte byteIdx())
    uint16 bitIdx() const noexcept { return static_cast<uint16>(unNrBits % 8u); }

  public:
    typedef size_t size_type;

    /// Constructor
    Packet() : unNrBits(0) { clear(); }
    /// clear all data
    void clear()
    {
      unNrBits = 0;
      bytes.fill(0);
    }
    /// add a bit (0 or 1)
    void addBit(uint8 unBit)
    {
      refByte(byteIdx()) = static_cast<uint8>((refByte(byteIdx()) << 1u) | unBit);
      unNrBits++;
    }
    /// get a single byte
    uint8& refByte(size_type idx) { return bytes[idx]; }
    const uint8& refByte(size_type idx) const { return bytes[idx]; }
    /// get the number of used bytes
    size_type getNrBytes() const noexcept { return Util::ceilT(unNrBits, static_cast<uint16>(8u)); }
    /// equality
    bool operator==(const Packet& p) const
    {
      bool bRet;
      uint16_t i;

      bRet = (p.unNrBits == unNrBits);
      for (i = 0; i < byteIdx(); i++)
      {
        bRet = bRet && (p.refByte(i) == refByte(i));
      }

      return bRet;
    }
  };
  
} // namespace Dcc

#endif // DCC_PACKET_H