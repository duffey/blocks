#ifndef CUBE_H_
#define CUBE_H_

#include <vector>
#include <cassert>		//assert

#include "SimulatedModel.h"
#include "Vector4.h"
#include "Matrix44.h"
#include "NxPhysics.h"

using namespace std;

/**
  * @brief As a child of SimulatedModel, Cube simply supplies the vertices and a collision hull to its parent's constructor given a specific size.
  * @see SimulatedModel
  */
class Cube : public SimulatedModel
{
	private:
		NxScene& physxScene;

	public:
		Cube(	NxScene& physxScene,
				GLfloat size = 1.0,
				const Matrix44& globalOrientation = Matrix44()) :
		
				SimulatedModel(Cube::createCubeActor(physxScene, size, globalOrientation),
				vector<Vector4>(),
				globalOrientation),

				physxScene(physxScene)
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
			physxScene.releaseActor(const_cast<NxActor&>(getActor()));
		}

	private:
		static NxActor& createCubeActor(NxScene& physxScene, const GLfloat& size, const Matrix44& globalOrientation = Matrix44())
		{
			NxBodyDesc bodyDesc;
			NxBoxShapeDesc boxDesc;
			NxActorDesc actorDesc;

			bodyDesc.angularDamping	= 0.5f;

			boxDesc.dimensions = NxVec3(size / 2.0, size / 2.0, size / 2.0);

			actorDesc.shapes.pushBack(&boxDesc);
			actorDesc.body = &bodyDesc;
			actorDesc.density = 10.0f;
			
			//NOTE: Fix this mess (possibly with a data() method for Matrix44)
			GLfloat temp[16];

			int index = 0;

			for(Matrix44::const_iterator i = globalOrientation.begin(); i != globalOrientation.end(); i++, index++)
				temp[index] = *i;

			NxMat34 orientation(NxMat33(NxVec3(temp[0], temp[1], temp[2]),
										NxVec3(temp[4], temp[5], temp[6]),
										NxVec3(temp[8], temp[9], temp[10])),
										NxVec3(temp[12], temp[13], temp[14]));

			actorDesc.globalPose = orientation;

			return *(physxScene.createActor(actorDesc));
		}
};

#endif /*CUBE_H_*/
