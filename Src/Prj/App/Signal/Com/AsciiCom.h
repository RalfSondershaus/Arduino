/**
 * @file Signal/Com/AsciiCom.h
 *
 * @brief Handles ASCII-based telegram communication for signal control.
 *
 * The AsciiCom class is responsible for receiving,
 * processing, and responding to ASCII telegrams related to signal configuration
 * and status queries. The supported telegrams allow setting and getting signal
 * aspects, blinks, targets, input classification, and change-over times, as well
 * as copying signal configurations between IDs.
 *
 * Example telegrams:
 * - SET_SIGNAL 0 ASPECTS 11000 00100 00000 00000 00000
 * - SET_SIGNAL 0 BLINKS  00110 11001 11111 00000 00000
 * - SET_SIGNAL 0 TARGETS ONBOARD pin ONBOARD pin ONB pin
 * - SET_SIGNAL 0 INPUT CLASSIFIED 1
 * - SET_SIGNAL 0 COT 10 20
 * - GET_SIGNAL 0 ASPECTS
 * - GET_SIGNAL 0 TARGETS
 * - SET_CLASSIFIER 1 PIN 5
 * - SET_CLASSIFIER 1 LIMITS 2 10 20
 * - SET_CLASSIFIER 1 DEBOUNCE 5
 * - GET_CLASSIFIER 1 PIN
 * - GET_CLASSIFIER 1 LIMITS
 * - GET_CLASSIFIER 1 DEBOUNCE
 *
 * @note The signal ID is a number from 0 to 63. Aspects and blinks are bit values.
 * Target ports can be ONBOARD, ONB, EXTERN, or EXT, followed by a pin number (0-31).
 * Change-over times are two values in [10 ms].
 * 
 * @note Parts of the documentation of this file was created by GitHub Copilot.
 * 
 * @copyright Copyright 2024-2025 Ralf Sondershaus
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

#ifndef ASCIICOM_H_
#define ASCIICOM_H_

#include <Std_Types.h>
#include <Com/Observer.h>
#include <Com/SerAsciiTP.h>
#include <Util/Array.h>

namespace com
{

  /**
   * Receives and processes ASCII telegrams for signal control.
   * 
   * This class listens to a SerAsciiTP instance and processes incoming telegrams
   * related to signal configuration and status queries. It supports commands for
   * setting and getting signal aspects, blinks, targets, input classification,
   * and change-over times, as well as configuring classifiers.
   * 
   * @section set_signal SET_SIGNAL Command Reference
   *
   * The `SET_SIGNAL` command is used to configure various aspects of a signal, such as its aspects, blink states, target pins, input classification, and change-over times.
   *
   * | Command | Description | Example Usage |
   * |---------|-------------|--------------|
   * | `SET_SIGNAL id ASPECTS [aspect]+` | Set the aspects for a signal. `[aspect]+` can include up to `kNrSignalAspects` aspects. If fewer than `kNrSignalAspects` aspects are provided, only the specified aspects are updated; the remaining aspects will stay unchanged. A `1` means that the corresponding LED is on, `0` means off. The order of the bits corresponds to the order of the pins, see `SET_SIGNAL id TARGETS`. | `SET_SIGNAL 0 ASPECTS 11000 00100 00110 11001 11111`<br>Sets aspects for signal 0. With respect to the example of `SET_SIGNAL id TARGETS` below, `11000`, the left-most `1` corresponds to pin 13, the right-most `0` to pin 9. |
   * | `SET_SIGNAL id BLINKS [blink]+` | Set blink for a signal. `[blink]+` can include up to `kNrSignalAspects` blinks, so a blink per aspect. If fewer than `kNrSignalAspects` blinks are provided, only the specified blinks are updated; the remaining blinks will stay unchanged. A `1` means that the corresponding LED starts to blink, `0` means LED on without blink. | `SET_SIGNAL 0 BLINKS 00000 00000 00000 00000 00000`<br>Signal 0 doesn't blink. |
   * | `SET_SIGNAL id TARGETS [(ONBOARD, ONB, EXTERN, EXT) pin]+` | Sets up to `NrSignalTargets` target ports for a signal. EXTERN (EXT) are currently not supported. `pin` is the Arduino pin number. | `SET_SIGNAL 0 TARGETS ONB 13 ONB 12 ONB 11 ONB 10 ONB 9`<br>Set 5 targets for signal 0: pins 13, 12, 11, 10, and 9. |
   * | `SET_SIGNAL id INPUT CLASSIFIED id` | Sets input type: type is classifier with ID `id`. | `SET_SIGNAL 0 INPUT CLASSIFIED 0`<br>Assigns classifier 0 to signal 0. |
   * | `SET_SIGNAL id COT t1 t2` | Sets change-over times (`t1` and `t2` use [10 ms]). | `SET_SIGNAL 0 COT 10 20`<br>Set change over time of signal 0 to 100 ms and blink change over time to 200 ms. |
   *
   * @note
   * - `[aspect]+` and `[blink]+` refer to sequences of bits, each representing the state of an aspect or blink for the signal.
   * - The order of bits in aspects and blinks corresponds to the order of the pins defined in the TARGETS command.
   * - Change-over times are specified in units of 10 ms.
   * 
    * @section GET_SIGNAL GET_SIGNAL Commands
    * @brief Commands to retrieve signal configuration and status.
    *
    * | Command | Description | Example Usage |
    * |---------|-------------|--------------|
    * | `GET_SIGNAL id ASPECTS` | Returns all `kNrSignalAspects` aspects for a signal. | `GET_SIGNAL 0 ASPECTS` |
    * | `GET_SIGNAL id BLINKS` | Returns all `kNrSignalAspects` blinks for a signal. | `GET_SIGNAL 0 BLINKS` |
    * | `GET_SIGNAL id TARGETS` | Returns all `kNrSignalTargets` target ports for the given signal id. | `GET_SIGNAL 0 TARGETS` |
    * | `GET_SIGNAL id INPUT` | Returns the classifier and classifier ID. | `GET_SIGNAL 0 INPUT` |
    * | `GET_SIGNAL id COT` | Returns change over time [10 ms] and blink change over time [10 ms]. | `GET_SIGNAL 0 COT` |
    *
    * @section SET_CLASSIFIER SET_CLASSIFIER Commands
    * @brief Commands to configure classifier parameters.
    *
    * | Command | Description | Example Usage |
    * |---------|-------------|--------------|
    * | `SET_CLASSIFIER id PIN pin` | Sets the Arduino analog input pin (ADC). | `SET_CLASSIFIER 0 PIN 56` |
    * | `SET_CLASSIFIER id LIMITS slot-id min max` | Sets the AD limits for the given slot of the classifier. A classifier supports `kNrClassifierClasses` classes. `min` and `max` are given in raw values. The AD values are `4*min` and `4*max`. | `SET_CLASSIFIER 0 LIMITS 1 85 102` |
    * | `SET_CLASSIFIER id DEBOUNCE t1` | Sets debounce time [10 ms] for a classifier. | `SET_CLASSIFIER 0 DEBOUNCE 5` |
    *
    * @section GET_CLASSIFIER GET_CLASSIFIER Commands
    * @brief Commands to retrieve classifier configuration.
    *
    * | Command | Description | Example Usage |
    * |---------|-------------|--------------|
    * | `GET_CLASSIFIER id PIN` | Gets the Arduino analog input pin (ADC). | `GET_CLASSIFIER 0 PIN` |
    * | `GET_CLASSIFIER id LIMITS` | Gets the AD limits for the given slot of the classifier. A classifier supports `kNrClassifierClasses` classes. `min` and `max` are given in raw values. The AD values are `4*min` and `4*max`. | `GET_CLASSIFIER 0 LIMITS` |
    * | `GET_CLASSIFIER id DEBOUNCE` | Gets debounce time [10 ms] for a classifier. | `GET_CLASSIFIER 0 DEBOUNCE` |
    */
  class AsciiCom : public Observer
  {
  public:
    using char_type = SerAsciiTP::telegram_base_type;
    using string_type = SerAsciiTP::string_type;
    using size_type = string_type::size_type;


  protected:
    /**
     * @brief Smart pointer to a SerAsciiTP object.
     *
     * Currently, just one observer is supported, so this pointer
     * is used to listen to the SerAsciiTP instance for incoming telegrams.
     */
    util::ptr<SerAsciiTP> asciiTP;
    /**
     * @brief Stores the response telegram as a string.
     *
     * This variable holds the response message received or to be sent
     * in ASCII communication. The type string_type is an alias
     * for util::basic_string class.
     * @note The length of this string is limited to kMaxLenTelegram.
     * @see SerAsciiTP::kMaxLenTelegram
     */
    string_type telegram_response;

  public:
    /**
     * @brief Default constructor for the AsciiCom class.
     *
     * Initializes a new instance of the AsciiCom class with default values.
     */
    AsciiCom() = default;

    /**
     * @brief Processes data for the ASCII communication interface.
     *
     * This method is called by AsciiTP to notify this observer of new data.
     *
     * @note This function overrides a virtual method from the base class.
     */
    void update() override;

    /**
     * @brief Attaches this object as a listener to the specified SerAsciiTP instance.
     *
     * This method sets the internal pointer to the provided SerAsciiTP object and
     * registers this object as an observer by calling the attach method on the SerAsciiTP instance.
     *
     * @param tp Reference to the SerAsciiTP object to listen to.
     */
    void listenTo(SerAsciiTP& tp) { asciiTP = &tp; tp.attach(*this); }

    /**
     * @brief Processes the given telegram and generates a response.
     *
     * This function takes an input telegram, performs the necessary processing,
     * and writes the result to the response parameter.
     *
     * @param telegram The input string containing the telegram to be processed.
     * @param response Reference to a string where the generated response will be stored.
     */
    void process(const string_type& telegram, string_type& response);

    /**
     * @brief Executes a single processing cycle for the ASCII communication interface.
     *
     * This method should be called periodically to handle communication tasks such as
     * receiving, parsing, and transmitting ASCII-formatted data. Typical actions performed
     * during the cycle include transmitting RTE monitoring values or outputting long lists.
     */
    void cycle();
  };
} // namespace com

#endif // ASCIICOM_H_