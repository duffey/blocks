#ifndef CUBE_H_
#define CUBE_H_

#include <vector>
#include <cassert>		//assert

#include "SimulatedModel.h"
#include "Vector4.h"
#include "Matrix44.h"

using namespace std;

/**
  * @brief As a child of SimulatedModel, Cube simply supplies the vertices and a collision hull to its parent's constructor given a specific size.
  * @see SimulatedModel
  */
class Cube : public SimulatedModel
{
	private:

	public:
		Cube(GLfloat size = 1.0,
				const Matrix44& globalOrientation = Matrix44()) :
		
				SimulatedModel(Cube::createCubeActor(size, globalOrientation),
				vector<Vector4>(),
				globalOrientation)
		{
			vector<Vector4> vertices;
			
			vertices.push_back(Vector4(size / 2.0, size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, -size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, -size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, -size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, -size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, -size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, -size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, -size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, -size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, -size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, -size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, -size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, -size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, -size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, -size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, -size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(size / 2.0, size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, -size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, -size / 2.0, -size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, -size / 2.0, size / 2.0, 1.0));
			vertices.push_back(Vector4(-size / 2.0, size / 2.0, size / 2.0, 1.0));
			
			vertices >> *this;
		}

		~Cube()
		{
		}

	private:
		static bool createCubeActor(const GLfloat& size, const Matrix44& globalOrientation = Matrix44())
		{
			return false;
		}
};

#endif /*CUBE_H_*/
