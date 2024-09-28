/**
 * @file Signal/Com/ComR.h
 *
 * @brief 
 *
 * @copyright Copyright 2023 Ralf Sondershaus
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
 * See <https://www.gnu.org/licenses/>.
 */

#ifndef COMR_H_
#define COMR_H_

#include <Std_Types.h>
#include <Util/bitset.h>

namespace com
{
  
  // UDP
  // 
  // bit  |0                            15|16                           31|
  //      |         source port           |       destination port        |
  // 
  // bit  |32                           47|48                           63|
  //      |            length             |            checksum           |
  // 
  // byte |64 ff
  //      | payload
  // 
  // length: payload + header
  // 
  // 
  // IPv4
  // 
  // bit  |0     3|4     7|8            15|16                           31|
  //      |Version|IHL    |   TOS         |          Length               |
  // 
  // bit  |32                           47|48 50|51                     63|
  //      |     Identification            |Flags|   Fragment Offset       |
  // 
  // bit  |64           71|72           79|80                           95|
  //      |     TTL       |  Protocol     |         Header Checksum       |
  // 
  // bit  |96                                                          127|
  //      |                  Source Address                               |
  // ...
  // 
  // Protocol: 17 = UDP
  // Length = payload + header
  // 
  // 
  // ISO CAN-TP
  // 
  // 0        PCI (Protocol Control Information)
  //          bit 4 - 7: code (0 = single frame, 1 = first frame, 2 = consecutive frame, 3 = flow control)
  //          bit 0 - 3: code == 0: length of payload bytes (0 ... 7)
  //                     code == 1: high nibble of 12 bit size
  //                     code == 2: index 1 ... 15
  //                     code == 3: Flag
  // 1        code == 1: PCI (cont'd), low byte of 12 bit size
  //          code == 3: block size
  //          else     : payload
  // 2, ...   payload
  // 
  // For UDS, payload contains UDS request SID, UDS subfunction byte, UDS DID, padding bytes.
  // 
  // 
  // For serial communication, Arduino uses a 16 byte buffer (for systems with RAM smaller 
  // than 1 KB) or a 64 byte buffer (else).
  // 
  // 
  // SerTP
  // 
  // bit  |0     3|4     7|8            15|16           23|24           31|32 ...      
  //      | Code  | P/F/I |    Length     | Total len hi* | Total len lo* | Payload
  // 
  // Single Message
  //      | 0     | Prot  |    Length     | Payload
  // 
  // First Message
  //      | 1     | Prot  |    Length     | Total len hi  | Total len lo  |
  //                           4
  // 
  // Flow Control Message
  //      | 2     | Flags |    Length     | Consecut Len  | Number        |
  //                           4
  //                0 = Continue to send  | 0 .. 256 bytes| 0  = send remaining messages without flow control
  //                1 = Wait              | Size of       | >0 = send number of message before waiting for the next flow control message
  //                2 = Overflow / abort  | payload in    |
  //                                      | consecutive   |
  //                                      | frames        |  
  // 
  // Consecutive Message
  //      | 3     | Index | Length        | Payload
  //                0 .. 15
  // 
  // Prot      Protocol identifier
  //    0 = ASCII
  //    1 = UDS
  //    2 = XCP
  // 
  // Length    Length of message in bytes including header and payload
  //    2 .. 16 bytes
  //    (16 bytes because of limited Serial input buffer size)
  // 
  // Total len hi    Size of total payload in bytes, high byte
  // Total len lo    Size of total payload in bytes, low byte
  //    0 ... 65535 bytes
  // 
  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  class SerTP
  {
  public:
    using size_type = size_t;
    typedef const uint8* const_data_pointer;

    /// Size of payload buffer
    static constexpr size_t kMaxLenPayload = 256U;

  protected:
    typedef enum
    {
      INIT = 0,
      START = 1,
      DECODE = 2
    } tDecodeState;
    tDecodeState decodeState;

    /// Protocol constants
    static constexpr uint8 kProtocolAscii = 0;
    static constexpr uint8 kProtocolUds = 1;

    /// Code constants
    static constexpr uint8 kCodeSingleFrame = 0;
    static constexpr uint8 kCodeFirstFrame = 1;
    static constexpr uint8 kCodeConsecutiveFrame = 2;
    static constexpr uint8 kCodeFlowControl = 3;

    size_type decodeMessage(const_data_pointer data, size_type count);
    
    size_type getMessageLength() const { return ucLength; }
    uint8 getMessageProtocol() const { return ucProtocol; }
    uint8 getMessageCode() const { return ucCode; }

    void forwardMessage();

    typedef struct
    {
      uint8 ucProtocol;   ///< The protocol, any of kProtocolXXX
      uint8 ucCode;       ///< The code, any of kCodeXXX
      uint8 ucLength;     ///< The length of the TP packet
      uint16 unTotalLength; ///< The length of the total payload (several TP packets)
      uint8 ucIdx;
    } tHeader;

    tHeader header;

    typedef struct
    {
      uint16 unSize; ///< Current position in aBuffer
      
    } tPduInfo;

    tPayload payload;

  public:
    /// Construct.
    SerTP() = default;

    /// Initialization
    void init();
    /// Data received, called from lower layers such as network layers
    void dataRx(const_data_pointer data, size_type count);
  };

} // namespace com

#endif // COMR_H_