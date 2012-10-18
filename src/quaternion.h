/***************************************************************************
 * Quaternion class written by BlackAxe / Kolor aka Laurent Schmalen in 1997
 * this class is freeware. you are fully allowed to use this class in non-
 * commercial products. Use in commercial environment is strictly prohibited
 * if you use this, i'll be happy if you greet/credit me and my group
 *
 * Official Version 0.9
 *
 * Laurent aka Blackaxe/KoLoR
 */


#ifndef BA_QUATERNION_H__
#define BA_QUATERNION_H__

#include <iostream>

#ifndef PI
#define PI 3.14159264
#endif

class Quaternion
{
  private:
    float W, X, Y, Z;      // components of a quaternion

  public:
    // default constructor
    Quaternion();
    Quaternion(const float, const float = 0.0, const float = 0.0, const float = 0.0);
  
    // print a quaternion using streams
    void PrintOn(std::ostream& = std::cout) const;

    // quaternion multiplication
    friend Quaternion operator * (const Quaternion&, const Quaternion&);
    const Quaternion& operator *= (const Quaternion&);

    // konjugiertes Quaternion
    const Quaternion& operator ~ ();

    // this function inverts the quaternion
    const Quaternion& operator - ();

    // this normalizes a quaternion 
    const Quaternion& Normalize();
    const Quaternion& FromAxis(const float, float, float, float);
    void ToMatrix(float matrix[9]) const;

    void Slerp(const Quaternion&, const Quaternion&, const float);

    // some additional Quaternion functions
    // getting the exponent of a quaternion
    const Quaternion& exp();
    const Quaternion& log();
};

inline std::ostream& operator << (std::ostream &strm, const Quaternion &q)
{
  q.PrintOn(strm);
  return strm;
}

#endif
