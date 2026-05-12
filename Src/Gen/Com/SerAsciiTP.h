/**
 * @file SerAsciiTP.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Serial ASCII transport protocol layer for the COM stack.
 *
 * @copyright Copyright 2023 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SERASCIITP_H_
#define SERASCIITP_H_

#include <Std_Types.h>
#include <Com/SerComDrv.h>
#include <Com/Subject.h>
#include <Util/String.h>
#include <Util/Ptr.h>

namespace com
{
  /**
   * @brief Serial ASCII transport protocol layer; assembles received bytes into telegrams.
   *
   * Bytes are read from a @ref SerComDrv driver one at a time during @ref cycle().
   * Printable characters (alphanumeric, space, punctuation) are appended to an
   * internal buffer until a control character (e.g. '\n') is received.
   * When the telegram is complete and non-empty, @ref Subject::notify() is called
   * so that all attached @ref Observer instances (e.g. @ref AsciiCom) can process it.
   *
   * Telegrams longer than @ref kMaxLenTelegram are discarded; an overflow message
   * is printed via the serial port and reception is suspended until the next
   * control character clears the overflow flag.
   *
   * @note @ref cycle() must be called periodically (e.g. every 1 ms) to drain the
   *       hardware receive buffer promptly.
   *
   * @see Subject, Observer, AsciiCom, SerComDrv
   */
  class SerAsciiTP : public Subject
  {
  public:
    static constexpr size_t kMaxLenTelegram = 64U; ///< Maximum number of characters in one telegram

    using telegram_base_type = char;                                                      ///< Character type used for telegram data
    using string_type = typename util::basic_string<kMaxLenTelegram, telegram_base_type>; ///< Fixed-capacity string holding up to kMaxLenTelegram characters

    string_type telegram_rawdata; ///< Buffer accumulating the current in-progress or last completed telegram

  protected:
    util::ptr<SerComDrv> driver; ///< Byte-level driver; may be null (no data received when null)
    bool bOverflow;              ///< Set when a telegram exceeds kMaxLenTelegram; cleared after the next control character

  public:
    /// Default constructor; no driver attached.
    SerAsciiTP() = default;

    /**
     * @brief Construct and attach the given serial driver.
     *
     * @param[in] drv Communication driver used to read and write serial bytes.
     */
    SerAsciiTP(SerComDrv& drv) : driver(&drv), bOverflow(false)
    {}

    /**
     * @brief Attach (or replace) the serial driver at run time.
     *
     * @param[in] drv Communication driver used to read and write serial bytes.
     */
    void setDriver(SerComDrv& drv) { driver = &drv; }

    /// Initialise the transport layer (currently a no-op; provided for lifecycle symmetry).
    void init();

    /**
     * @brief Read available bytes from the driver and assemble them into a telegram.
     *
     * Must be called cyclically. Calls @ref Subject::notify() when a complete,
     * non-empty telegram has been received (i.e. a control character was encountered).
     *
     * @note Telegrams that exceed @ref kMaxLenTelegram are discarded; an error
     *       message is printed via @ref hal::serial.
     */
    void cycle();

    /// Return a const reference to the last completed (or in-progress) telegram buffer.
    const string_type& getTelegram() const noexcept { return telegram_rawdata; }

    /**
     * @brief Transmit @p telegram to the serial port followed by CR+LF.
     *
     * Has no effect if no driver is attached.
     *
     * @param[in] telegram String to transmit.
     */
    void transmitTelegram(const string_type& telegram);
  };

} // namespace com

#endif // COMR_H_