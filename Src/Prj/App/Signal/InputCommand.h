/**
 * @file Signal/InputCommand.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Implements server runable to get a command.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SIGNAL_INPUT_COMMAND_H_
#define SIGNAL_INPUT_COMMAND_H_

#include <Prj_Types.h>

namespace signal
{
  // -----------------------------------------------------------------------------------
  /// Read AD values, classify and write classified values onto the RTE.
  // -----------------------------------------------------------------------------------
  class InputCommand
  {
  public:
    uint8 getCmd(struct signal::input_cmd in);
  };
} // namespace signal

#endif // SIGNAL_INPUT_COMMAND_H_
