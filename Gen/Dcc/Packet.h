/**
  * @file DccPacket.h
  */
#ifndef DCC_PACKET_H
#define DCC_PACKET_H

namespace Dcc
{
  // -----------------------------------------------------
  /// A packet has 0 ... MaxBytes bytes.
  // -----------------------------------------------------
  template<int MaxBytes = 6>
  class Packet
  {
  protected:
    constexpr int maxbytes() { return MaxBytes; }
  public:
    /// Constructor
    Packet() : unNrBits(0), unNrRcv(0) { clear(); }
    /// number of bits received
    unsigned int unNrBits;
    /// how often was this packet received?
    unsigned int unNrRcv;
    /// byte array
    unsigned char aBytes[MaxBytes];
    /// clear all data
    void clear()
    {
      int idx;

      unNrBits = 0;

      for (idx = 0; idx < MaxBytes; idx++)
      {
        aBytes[idx] = (unsigned char)0u;
      }
    }
    /// Return current byte index (index into array of bytes)
    constexpr unsigned int byteIdx() { return unNrBits / 8u; }
    /// Return current bit index (in current byte byteIdx())
    constexpr unsigned int bitIdx() { return unNrBits % 8u; }
    /// add a bit (0 or 1)
    void addBit(unsigned int unBit)
    {
      aBytes[byteIdx()] = (aBytes[byteIdx()] << 1u) | unBit;
      unNrBits++;
    }
    /// get a single byte
    constexpr unsigned char refByte(unsigned int idx) { return aBytes[idx]; }
    /// get the number of used bytes
    constexpr unsigned int getNrBytes() { return (unNrBits + 7u) / 8u; }
    /// equality
    constexpr bool operator==(const Packet& p)
    {
      bool bRet;
      unsigned int i;

      bRet = (p.unNrBits == unNrBits);
      for (i = 0; i < byteIdx(); i++)
      {
        bRet = bRet && (p.aBytes[i] == aBytes[i]);
      }

      return bRet;
    }
  };
  
} // namespace Dcc

#endif // DCC_PACKET_H