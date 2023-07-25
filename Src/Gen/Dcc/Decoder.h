/**
  * @file Decoder.h
  * @author Ralf Sondershaus
  *
  * @brief DCC Decoder for Arduino
  * 
  * Declares class dcc::Decoder with
  * - setup
  * - loop
  * 
  * @copyright Copyright 2018 - 2023 Ralf Sondershaus
  *
  * This program is free software: you can redistribute it and/or modify it
  * under the terms of the GNU General Public License as published by the
  * Free Software Foundation, either version 3 of the License, or (at your
  * option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#ifndef DCC_DECODER_H
#define DCC_DECODER_H

#include <Std_Types.h>
#include <Dcc/BitExtractor.h>
#include <Dcc/PacketExtractor.h>

namespace dcc
{
  // ---------------------------------------------------------------------
  /// Main class
  // ---------------------------------------------------------------------
  class Decoder
  {
  public:
    typedef BitExtractorConstants<> BitExtractorConstantsType;
    typedef PacketExtractor<> PacketExtractorType;
    typedef BitExtractor<BitExtractorConstantsType, PacketExtractorType> BitExtractorType;

    typedef PacketExtractorType::PacketType PacketType;

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
} // namespace dcc

#endif // DCC_DECODER_H