/**
 * @file Rte_Type_Runable.h
 *
 * @author Ralf Sondershaus
 *
 * @brief The Runable class is a proxy class to make a member function a runable.
 * 
 * @copyright Copyright 2020 - 2022 Ralf Sondershaus
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

#ifndef RTE_TYPE_RUNABLE_H_
#define RTE_TYPE_RUNABLE_H_

namespace rte
{
  // ----------------------------------------------
  /// Interface class for runables.
  // ----------------------------------------------
  class Runable
  {
  public:
    /// Default constructor
    Runable() {}
    /// Default destructor
    virtual ~Runable() {}
    /// Main execution function
    virtual void run(void) = 0;
  };
  
  /// Call a member function of an object (instance of a class).
  /// We use this macro because AVR GCC doesn't seem to support std::invoke.
  #define CALL_MEMBER_FUNC(obj,ptrToMemberFunc)  ((obj).*(ptrToMemberFunc))

  // ----------------------------------------------
  /// Make a Runable for a member function of a class
  // ----------------------------------------------
  template<class C>
  class TRunable : public Runable
  {
  public:
    /// The object is of this type
    typedef C class_type;
    /// Reference to the object
    typedef class_type& class_type_reference;
    /// Type of a pointer to a member function of class class_type
    typedef void (class_type::*member_func_type)(void);
  protected:
    /// The object
    class_type_reference obj;
    /// The pointer to a member function
    member_func_type func;
  public:
    /// Constructor
    TRunable(class_type_reference o, member_func_type f) : obj(o), func(f) {}
    /// Main execution function
    virtual void run(void) override { CALL_MEMBER_FUNC(obj, func)(); }
  };
} // namespace rte

#endif /* RTE_TYPE_RUNABLE_H_ */
