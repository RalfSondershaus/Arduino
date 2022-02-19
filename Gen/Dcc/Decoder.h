/// @file Decoder.h
///
/// DCC Decoder for Arduino
///
/// Declares class Dcc::Decoder with
/// - setup
/// - loop

#ifndef DCC_DECODER_H
#define DCC_DECODER_H

#include <Dcc/BitExtractor.h>
#include <Dcc/PacketExtractor.h>
#include <Util/Array.h>

using namespace Util;

namespace Dcc
{
  // ---------------------------------------------------------------------
  /// Main class
  // ---------------------------------------------------------------------
  class Decoder
  {
  public:
    typedef BitExtractorConstants<> MyBitExtractorConstants;
    typedef PacketExtractor<> MyPacketExtractor;
    typedef MyPacketExtractor::MyPacket MyPacket;
    typedef BitExtractor< MyBitExtractorConstants, MyPacketExtractor> MyBitExtractor;
    /// This handler is called if a new packet is received
    typedef MyPacketExtractor::HandlerIfc HandlerIfc;

  protected:

    /// The state machine for short and long half bits (-> detecting "1" and "0" and INVALID)
    MyBitExtractor bitExtr;
    /// The packet extractor
    MyPacketExtractor pktExtr;

  public:
    /// Constructor
    Decoder(HandlerIfc& hifc) : pktExtr(hifc), bitExtr(pktExtr) {}
    /// Desctructor
    ~Decoder() {}

    /// Initialize with interrupt pin
    void setup(unsigned int unIntPin);
    /// Loop
    void loop();

    /// for debugging: number of interrupt (ISR) calls
    unsigned int getDebugVal(int i);
  };
} // namespace Dcc

#endif // DCC_DECODER_H