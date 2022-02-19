/**
  * @file UtilArray.h
  */
#ifndef UTIL_ARRAY_H
#define UTIL_ARRAY_H

namespace Util
{
  // ---------------------------------------------------
  /// A simple container class which supports
  /// - begin
  /// - end
  /// - size
  /// - push_back (max number of elements is limited, no dynamic memory allocation)
  /// and other functions as known from STL containers
  // ---------------------------------------------------
  template<class MemberClass, int MaxSize>
  class Array
  {
  public:
    /// MemberClass
    typedef MemberClass ThisMemberClass;
    /// This class
    typedef Array<MemberClass, MaxSize> This;
    /// iterators
    typedef MemberClass *       iterator;
    typedef const MemberClass * const_iterator;
    /// Constructor
    Array() : unSize(0)
    {}
    /// Destructor
    virtual ~Array()
    {}
    /// begin
    iterator begin() { return (unSize > 0) ? (&(aMembers[0])) : ((iterator)0); }
    constexpr const_iterator begin() const { return (unSize > 0) ? (&(aMembers[0])) : ((iterator)0); }
    /// end
    iterator end() { return (unSize > 0) ? (&(aMembers[unSize])) : ((iterator)0); }
    constexpr const_iterator end() const { return (unSize > 0) ? (&(aMembers[unSize])) : ((iterator)0); }
    /// push back
    void push_back(const MemberClass& m)
    {
      if (unSize < MaxSize)
      {
        aMembers[unSize] = m;
        unSize++;
      }
    }
    /// back
    ThisMemberClass& back() { return aMembers[unSize - 1u]; }
    constexpr const ThisMemberClass& back() const { return aMembers[unSize - 1u]; }
    /// size
    constexpr unsigned int size() const { return unSize; }
    /// clear
    void clear() { unSize = 0u; }
    /// find
    constexpr iterator find(const ThisMemberClass& p)
    {
      iterator it;
      for (it = begin(); it != end(); it++)
      {
        if (*it == p)
        {
          break;
        }
      }
      return it;
    }
  protected:
    /// members
    ThisMemberClass aMembers[MaxSize];
    /// number of used elements
    unsigned int unSize;
  };

} // namespace

#endif // UTIL_ARRAY_H