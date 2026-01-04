/**
 * @file Signal/InputCommand.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief Implements server runable to get a command.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Rte/Rte.h>
#include <InputCommand.h>

namespace signal
{
  uint8 InputCommand::getCmd(struct signal::input_cmd in)
  {
    uint8 cmd = signal::kInvalidCmd;
    using size_type = rte::classified_values_array::size_type;

    if (in.type == signal::input_cmd::kAdc)
    {
      const size_type pos = static_cast<size_type>(in.idx);
      if (rte::ifc_classified_values::boundaryCheck(pos))
      {
        rte::ifc_classified_values::readElement(pos, cmd);
      }
    }
    else if (in.type == signal::input_cmd::kDcc)
    { 
      const size_type pos = static_cast<size_type>(in.idx);
      if (rte::ifc_dcc_commands::boundaryCheck(pos))
      {
        rte::ifc_dcc_commands::readElement(pos, cmd);
      }
    }
    else
    {
      // intentionally left empty
    }

    return cmd;
  }

} // namespace signal
