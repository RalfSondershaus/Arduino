/**
 * @file Signal/InputCommand.cpp
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

#include <Rte/Rte.h>
#include "InputCommand.h"

namespace signal
{
  InputCommand::cmd_type InputCommand::getCmd(cal::input_type in)
  {
    cmd_type cmd = rte::kInvalidCmd;

    if (in.type == cal::input_type::kClassified)
    {
      const rte::classified_values_array::size_type pos = static_cast<rte::classified_values_array::size_type>(in.idx);
      if (rte::ifc_classified_values::boundaryCheck(pos))
      {
        rte::ifc_classified_values::readElement(pos, cmd);
      }
    }

    return cmd;
  }

} // namespace signal
