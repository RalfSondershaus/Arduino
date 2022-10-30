/**
  * @file Decoder.h
  *
  * @author Ralf Sondershaus
  *
  * @brief DCC Decoder for Arduino
  *
  * Declares class Dcc::Decoder with
  * - setup
  * - loop
  */

#ifndef DCC_DECODER_H
#define DCC_DECODER_H

#include <Std_Types.h>
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
    typedef BitExtractorConstants<> BitExtractorConstantsType;
    typedef PacketExtractor<> PacketExtractorType;
    typedef PacketExtractorType::PacketType PacketType;
    typedef BitExtractor<BitExtractorConstantsType, PacketExtractorType> BitExtractorType;
    /// Such a handler is called if a new packet is received
    typedef PacketExtractorType::HandlerIfc HandlerIfc;

  protected:

    /// The state machine for short and long half bits (-> detecting "1" and "0" and INVALID)
    BitExtractorType bitExtr;
    /// The packet extractor
    PacketExtractorType pktExtr;

  public:
    /// Constructor
    Decoder(HandlerIfc& hifc) : pktExtr(hifc), bitExtr(pktExtr) {}
    /// Desctructor
    ~Decoder() {}

    /// Initialize with interrupt pin
    void setup(uint8 ucIntPin);
    /// Loop
    void loop();

    /// for debugging: number of interrupt (ISR) calls
    unsigned int getDebugVal(int i);
  };
} // namespace Dcc

#endif // DCC_DECODER_H