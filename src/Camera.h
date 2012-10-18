
//GlEW and GLFW
#include "glew.h"
#include "wglew.h"
#include "glfw.h"
#include "glut.h"

#include "math3d.h"
#include "quaternion.h"
#include "Vector.h"

class Camera
{
public:
	Camera()
	{}

	void
	setPos(float x, float y, float z);





private:
	Quaternion _rot,forward;
	Vec3f _pos;


};