/**
 * @file Signal/InputCommand.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Implements server runable to get a command.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
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

#ifndef SIGNAL_INPUT_COMMAND_H_
#define SIGNAL_INPUT_COMMAND_H_

#include <Rte/Rte_Type.h>
#include <Cal/CalM_Type.h>

namespace signal
{
  // -----------------------------------------------------------------------------------
  /// Read AD values, classify and write classified values onto the RTE.
  // -----------------------------------------------------------------------------------
  class InputCommand
  {
  public:
    using cmd_type = rte::cmd_type;

    cmd_type getCmd(cal::input_type in);
  };
} // namespace signal

#endif // SIGNAL_INPUT_COMMAND_H_
