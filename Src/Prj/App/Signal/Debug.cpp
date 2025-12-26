/**
 * @file Debug.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief
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

#include <Debug.h>

namespace debug
{
    /**
     * @brief The verbosity level for debug output
     */
    static uint8 verbose_level = 0;
    /**
     * @brief Enable debug output with given verbosity level
     * 
     * @param verbosity The verbosity level to set for debug output
     */
    void enable(uint8 verbosity)
    {
        verbose_level = verbosity;
    }

    /**
     * @brief Check if a message with given level shall be printed
     * 
     * @param msg_level The message level
     * @return true Print the message
     * @return false Do not print the message
     */
    bool shall_print(uint8 msg_level) noexcept
    {
        return (msg_level <= verbose_level);
    }

}
