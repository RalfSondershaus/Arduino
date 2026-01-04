/**
 * @file Runable.h
 * @author Ralf Sondershaus
 *
 * @brief Interface class Runable
 * 
 * @copyright 2018 - 2022 Ralf Sondershaus
 * 
 * SPDX-License-Identifier: Apache-2.0
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
