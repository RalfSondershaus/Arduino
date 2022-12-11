// ----------------------------------------------------------------------------
/// @file vecmath.h
/// @author Ralf Sondershaus
///
/// Declares:
/// - class Vec2D: A 2D vector
/// - class Mat2D: A 2x2 matrix
// ----------------------------------------------------------------------------

#ifndef VECMATH_H
#define VECMATH_H

// ----------------------------------------------------------------------------
/// - class Vec2D: A 3D vector
// ----------------------------------------------------------------------------
template<typename T>
class Vec3D
{
public:
  typedef T BaseType;
  typedef Vec3D<T> This;

  T c[3] = { 0, 0, 0 };

  Vec3D() = default;
  Vec3D(T x, T y, T z) { c[0] = x; c[1] = y; c[2] = z; }
  Vec3D(const T coords[3]) { c[0] = coords[0]; c[1] = coords[1]; c[2] = coords[2]; }
  Vec3D(const This& v) { c[0] = v.c[0]; c[1] = v.c[1]; c[2] = v.c[2]; }
  ~Vec3D() {}

  const T& operator[](int i) const { return c[i]; }
  T& operator[](int i) { return c[i]; }
  //T    get(int i) const { return c[i]; }
  const T& x() const { return c[0]; }
  const T& y() const { return c[1]; }
  const T& z() const { return c[2]; }
  T& x() { return c[0]; }
  T& y() { return c[1]; }
  T& z() { return c[2]; }
  void set(int i, T v)        { c[i] = v; }
  void set(T x, T y, T z)     { c[0] = x; c[1] = y; c[2] = z; }
  void set(const T coords[3]) { c[0] = coords[0]; c[1] = coords[1]; c[2] = coords[2]; }
  void zero()                 { c[0] = c[1] = c[2] = (T)0; }
  
  /// dot product
  T operator|(const This& v) const { return (c[0] * v.c[0]) + (c[1] * v.c[1]) + (c[2] * v.c[2]); }

  /// copy assignment
  This& operator=(const This& v) { c[0] = v.c[0]; c[1] = v.c[1]; c[2] = v.c[2]; return *this; }
  /// move assignment
  This& operator=(const This&& v) { c[0] = v.c[0]; c[1] = v.c[1]; c[2] = v.c[2]; return *this; }

  /// addition, substraction, scaling
  This& operator+=(const This& rhs) { c[0] += rhs.c[0]; c[1] += rhs.c[1]; c[2] += rhs.c[2]; return *this; }
  This& operator-=(const This& rhs) { c[0] -= rhs.c[0]; c[1] -= rhs.c[1]; c[2] -= rhs.c[2]; return *this; }
  This& operator*=(const BaseType f) { c[0] *= f; c[1] *= f; c[2] *= f; return *this; }
  friend This operator+(This lhs, const This& rhs) { lhs += rhs; return lhs; }
  friend This operator-(This lhs, const This& rhs) { lhs -= rhs; return lhs; }
  friend This operator*(const BaseType f, This rhs) { rhs *= f; return rhs; }
};

// ----------------------------------------------------------------------------
/// - class Mat2D: A 2x2 matrix
// ----------------------------------------------------------------------------
template<typename T>
class Mat3D
{
public:
  typedef T BaseType;
  typedef Mat3D<T> This;
  typedef Vec3D<T> V3D;

  V3D row[3];

  Mat3D() {}
  Mat3D(const Mat3D&) = default;
  ~Mat3D() = default;

  void zero()       { row[0].zero(); row[1].zero(); row[2].zero(); }
  void identity()   { row[0] = V3D(1, 0, 0); row[1] = V3D(0, 1, 0); row[2] = V3D(0, 0, 1); }
  void translate(T x, T y) { row[0][2] = x; row[1][2] = y; }
  void translate(const V3D& v) { row[0][2] = v[0]; row[1][2] = v[1]; row[2][2] = v[2]; }

  V3D getColumnX() const { V3D v; v[0] = row[0][0]; v[1] = row[1][0]; v[2] = row[2][0]; return v; }
  V3D getColumnY() const { V3D v; v[0] = row[0][1]; v[1] = row[1][1]; v[2] = row[2][1]; return v; }
  V3D getColumnZ() const { V3D v; v[0] = row[0][2]; v[1] = row[1][2]; v[2] = row[2][2]; return v; }
  
  void set(T v00, T v01, T v02, T v10, T v11, T v12, T v20, T v21, T v22)
  {
    row[0][0] = v00; row[0][1] = v01; row[0][2] = v02;
    row[1][0] = v10; row[1][1] = v11; row[1][2] = v12;
    row[2][0] = v20; row[2][1] = v21; row[2][2] = v22;
  }
  void set(const V3D& r0, const V3D& r1, const V3D& r2) { row[0] = r0; row[1] = r1; row[2] = r2; }
  const V3D& operator[](int r) const { return row[r]; }
  V3D& operator[](int r) { return row[r]; }
  V3D operator*(const V3D& v) const { return V3D(row[0] | v, row[1] | v, row[2] | v); }
  This operator=(const This& m) { row[0] = m.row[0]; row[1] = m.row[1]; row[2] = m.row[2]; return *this;  }
  This operator*(const This& m) const 
  {
    V3D col0 = m.getColumnX();
    V3D col1 = m.getColumnY();
    V3D col2 = m.getColumnZ();
    This r;
    r.row[0][0] = row[0] | col0;
    r.row[0][1] = row[0] | col1;
    r.row[0][2] = row[0] | col2;
    r.row[1][0] = row[1] | col0;
    r.row[1][1] = row[1] | col1;
    r.row[1][2] = row[1] | col2;
    r.row[2][0] = row[2] | col0;
    r.row[2][1] = row[2] | col1;
    r.row[2][2] = row[2] | col2;
    return r;
  }
  This getTranspose() const
  {
    This m;
    m.row[0][0] = row[0][0];
    m.row[0][1] = row[1][0];
    m.row[0][2] = row[2][0];
    m.row[1][0] = row[0][1];
    m.row[1][1] = row[1][1];
    m.row[1][2] = row[2][1];
    m.row[2][0] = row[0][2];
    m.row[2][1] = row[1][2];
    m.row[2][2] = row[2][2];
    return m;
  }
};

#endif //VECMATH_H