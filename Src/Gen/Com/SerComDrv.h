/**
 * @file SerComDrv.h
 *
 * @author Ralf Sondershaus
 *
 * @brief HAL adapter exposing hal::serial as a byte-level serial communication driver.
 *
 * @copyright Copyright 2023 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COMDRV_H_
#define COMDRV_H_

#include <Std_Types.h>
#include <Util/Fix_Queue.h>
#include <Hal/Serial.h>

namespace com
{
  /**
   * @brief Byte-level serial driver; thin adapter over @ref hal::serial.
   *
   * Provides a uniform interface so that @ref SerAsciiTP can read from and
   * write to the hardware serial port without a direct dependency on the HAL.
   *
   * @see SerAsciiTP, hal::serial
   */
  class SerComDrv
  {
  public:
    using size_type = size_t;   ///< Unsigned size / byte-count type
    using base_type = uint8_t;  ///< Type of a single serial byte

  public:
    SerComDrv() = default;

    /**
     * @brief Open the serial port at the given baud rate.
     *
     * @param[in] baudrate Desired baud rate (e.g. 9600, 115200).
     */
    void begin(unsigned long baudrate) { hal::serial::begin(baudrate); }

    /// Return the number of bytes available to read from the serial port (0 if none).
    int available(void)             { return hal::serial::available(); }

    /// Return the next incoming byte (0…255), or -1 if no data is available.
    int read(void) const noexcept   { return hal::serial::read(); }

    /**
     * @brief Write a null-terminated string to the serial port.
     *
     * @param[in] p Null-terminated character string to transmit.
     * @return Number of bytes written.
     */
    int write(const char* p)        { return hal::serial::print(p); }

  };
} // namespace com

#endif // COMDRV_H_