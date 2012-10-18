
#include <iostream>
#include <math.h>
#include <float.h>
#include "quaternion.h"

Quaternion::Quaternion()
{
  W = 1.0;
  X = 0.0;
  Y = 0.0;
  Z = 0.0;
}

Quaternion::Quaternion(const float w, const float x, const float y, const float z)
{
  W = w;
  X = x;
  Y = y;
  Z = z;
}

void Quaternion::PrintOn(std::ostream &strm) const
{
  strm << W << " + " << X << "i + " << Y << "j + " << Z << "k";
}

Quaternion operator * (const Quaternion &a, const Quaternion &b)
{
  float w,x,y,z;

  w = a.W*b.W - (a.X*b.X + a.Y*b.Y + a.Z*b.Z);
  
  x = a.W*b.X + b.W*a.X + a.Y*b.Z - a.Z*b.Y;
  y = a.W*b.Y + b.W*a.Y + a.Z*b.X - a.X*b.Z;
  z = a.W*b.Z + b.W*a.Z + a.X*b.Y - a.Y*b.X;

  return Quaternion(w,x,y,z); 
}

const Quaternion& Quaternion::operator *= (const Quaternion &q)
{
  float w = W*q.W - (X*q.X + Y*q.Y + Z*q.Z);

  float x = W*q.X + q.W*X + Y*q.Z - Z*q.Y;
  float y = W*q.Y + q.W*Y + Z*q.X - X*q.Z;
  float z = W*q.Z + q.W*Z + X*q.Y - Y*q.X;

  W = w;
  X = x;
  Y = y;
  Z = z;
  return *this;
}

const Quaternion& Quaternion::operator ~ ()
{
  X = -X;
  Y = -Y;
  Z = -Z;
  return *this;
}

const Quaternion& Quaternion::operator - ()
{
  float norme = sqrt(W*W + X*X + Y*Y + Z*Z);
  if (norme == 0.0)
	norme = 1.0;

  float recip = 1.0 / norme;

  W =  W * recip;
  X = -X * recip;
  Y = -Y * recip;
  Z = -Z * recip;

  return *this;
}

const Quaternion& Quaternion::Normalize()
{
  float norme = sqrt(W*W + X*X + Y*Y + Z*Z);
  if (norme == 0.0)
	{
	  W = 1.0; 
	  X = Y = Z = 0.0;
	}
  else
	{
	  float recip = 1.0/norme;

	  W *= recip;
	  X *= recip;
	  Y *= recip;
	  Z *= recip;
	}
  return *this;
}

const Quaternion& Quaternion::FromAxis(const float Angle, float x, float y, float z)
{
  float omega, s, c;
  int i;
	
  s = sqrt(x*x + y*y + z*z);
  
  if (fabs(s) > FLT_EPSILON)
	{
	  c = 1.0/s;
	  
	  x *= c;
	  y *= c;
	  z *= c;

	  omega = -0.5f * Angle;
	  s = (float)sin(omega);

	  X = s*x;
	  Y = s*y;
	  Z = s*z;
	  W = (float)cos(omega);
	}
  else
	{
	  X = Y = 0.0f;
	  Z = 0.0f;
	  W = 1.0f;
	}
  Normalize();
  return *this;
}


void Quaternion::ToMatrix(float matrix[9]) const
{
	matrix[0] = 1.0 - 2*Y*Y - 2*Z*Z;
	matrix[1] = 2*X*Y + 2*W*Z;
	matrix[2] = 2*X*Z - 2*W*Y;

	matrix[3] = 2*X*Y - 2*W*Z;
	matrix[4] = 1.0 - 2*X*X - 2*Z*Z;
	matrix[5] = 2*Y*Z + 2*W*X;

	matrix[6] = 2*X*Z + 2*W*Y;
	matrix[7] = 2*Y*Z - 2*W*X;
	matrix[8] = 1.0 - 2*X*X - 2*Y*Y;

	/*
  matrix[0][0] = 1.0 - 2*Y*Y - 2*Z*Z;
  matrix[1][0] = 2*X*Y + 2*W*Z;
  matrix[2][0] = 2*X*Z - 2*W*Y;

  matrix[0][1] = 2*X*Y - 2*W*Z;
  matrix[1][1] = 1.0 - 2*X*X - 2*Z*Z;
  matrix[2][1] = 2*Y*Z + 2*W*X;

  matrix[0][2] = 2*X*Z + 2*W*Y;
  matrix[1][2] = 2*Y*Z - 2*W*X;
  matrix[2][2] = 1.0 - 2*X*X - 2*Y*Y;
  */
}


void Quaternion::Slerp(const Quaternion &a,const Quaternion &b, const float t)
{
  float omega, cosom, sinom, sclp, sclq;
  int i;


  cosom = a.X*b.X + a.Y*b.Y + a.Z*b.Z + a.W*b.W;


  if ((1.0f+cosom) > FLT_EPSILON)
	{
	  if ((1.0f-cosom) > FLT_EPSILON)
		{
		  omega = acos(cosom);
		  sinom = sin(omega);
		  sclp = sin((1.0f-t)*omega) / sinom;
		  sclq = sin(t*omega) / sinom;
		}
	  else
		{
		  sclp = 1.0f - t;
		  sclq = t;
		}

	  X = sclp*a.X + sclq*b.X;
	  Y = sclp*a.Y + sclq*b.Y;
	  Z = sclp*a.Z + sclq*b.Z;
	  W = sclp*a.W + sclq*b.W;

	}
  else
	{
	  X =-a.Y;
	  Y = a.X;
	  Z =-a.W;
	  W = a.Z;

	  sclp = sin((1.0f-t) * PI * 0.5);
	  sclq = sin(t * PI * 0.5);

	  X = sclp*a.X + sclq*b.X;
	  Y = sclp*a.Y + sclq*b.Y;
	  Z = sclp*a.Z + sclq*b.Z;

	}
}

const Quaternion& Quaternion::exp()
{                               
  float Mul;
  float Length = sqrt(X*X + Y*Y + Z*Z);

  if (Length > 1.0e-4)
	Mul = sin(Length)/Length;
  else
	Mul = 1.0;

  W = cos(Length);

  X *= Mul;
  Y *= Mul;
  Z *= Mul; 

  return *this;
}

const Quaternion& Quaternion::log()
{
  float Length;

  Length = sqrt(X*X + Y*Y + Z*Z);
  Length = atan(Length/W);

  W = 0.0;

  X *= Length;
  Y *= Length;
  Z *= Length;

  return *this;
}

