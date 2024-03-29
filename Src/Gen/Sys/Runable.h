/**
 * @file Runable.h
 * @author Ralf Sondershaus
 *
 * @brief Interface class Runable
 * 
 * @copyright
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
 *
 * Copyright 2018 - 2022 Ralf Sondershaus
 */

#ifndef RUNABLE_H_
#define RUNABLE_H_

namespace Sys
{
  /// Interface class for runables
  class Runable
  {
  public:
    /// Default constructor
    Runable() {}
    /// Default destructor
    virtual ~Runable() {}
    /// Initialization at system start
    virtual void init(void) = 0;
    /// Main execution function
    virtual void run(void) = 0;
  };

} // namespace Sys

#endif /* RUNABLE_H_ */
