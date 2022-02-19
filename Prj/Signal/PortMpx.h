/**
 * Multiplexer for ports.
 */

#ifndef PORTMPX_H__
#define PORTMPX_H__

// -----------------------------------------------------------------------------------
/// Two-multiplexer for ports.
// -----------------------------------------------------------------------------------
template<class IFC>
class PortMpx2
{
protected:
  int nCur;

public:
  const IFC& mPort1;
  const IFC& mPort2;

  PortMpx2(const IFC& p1, const IFC& p2) : nCur(0), mPort1(p1), mPort2(p2)
  {}
  ~PortMpx2() 
  {}
  int getCurrentIdx() const { return nCur; }
  void setCurrentIdx(int nIdx)
  {
    if (nIdx < N)
    {
      nCur = nIdx;
    }
  }
  const IFC& ref() const
  {
    switch (nCur)
    {
    case 0: return mPort1;
    default: return mPort2;
    }
  }
};

#endif // PORTMPX_H__
