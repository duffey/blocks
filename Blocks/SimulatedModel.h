#ifndef SIMULATEDMODEL_H_
#define SIMULATEDMODEL_H_

#include <vector>		//vector
#include <utility>		//pair
#include <algorithm>	//remove and swap
#include <fstream>		//ifstream
#include <cassert>		//assert
#include <iostream>		//istream and ostream
#include <stdexcept>	//runtime_error
#include <string>		//string
#include <cmath>		//abs

#include "Vector4.h"
#include "Matrix44.h"
#include "NxPhysics.h"

using namespace std;

/**
  * @brief This class contains similar methods to Model, but adds support for physics simulation .
  */
class SimulatedModel
{
	private:
		static const GLfloat moveThreshold;

		typedef pair<Vector4, Vector4> Edge;
		
		enum { x, y, z, w };
		
		NxActor& actor;
		vector<Vector4> vertices;
		/* This matrix is no longer used as in the Model class.
		 * It is simply updated to the NxActor's orientation each time a draw method is called.
		 * Therefore, since both draw and drawShadowVolume are const and we don't consider the NxActor's orientation to be changing the model in any way,
		 * we declare this auxiliary matrix mutable.
		 */
		mutable Matrix44 globalOrientation;
		Vector4 color;

		//Used in computing and caching the shadow volume
		mutable bool hasMoved;
		mutable Vector4 previousLightPosition;
		mutable vector<Vector4> normals;
		mutable vector<Vector4> shadowVolumeVertices;
		mutable vector<Vector4> transformedVertices;
		mutable vector<Edge> edges;

	public:
		SimulatedModel(NxActor& actor, const vector<Vector4>& vertices = vector<Vector4>(), const Matrix44& globalOrientation = Matrix44(), Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0)) : actor(actor), vertices(vertices), globalOrientation(globalOrientation), color(color), hasMoved(true), previousLightPosition() { vertices >> *this; }
		
		SimulatedModel(NxActor& actor, const char* filePath, const Matrix44& globalOrientation = Matrix44(), Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0)) : actor(actor), globalOrientation(globalOrientation), color(color), hasMoved(true), previousLightPosition() { filePath >> *this; }

		virtual ~SimulatedModel() {}
		
		void draw() const
		{
			//Get the simulated results back and update the model's orientation
			updateLocation();

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

				glMultMatrixf(globalOrientation);
				glColor4fv(color.data());
				glBegin(GL_TRIANGLES);

					for(vector<Vector4>::const_iterator i = vertices.begin(), j = normals.begin(); i != vertices.end() && j != normals.end(); i += 3, j++)
					{
						glNormal3fv(j -> data());
						glVertex4fv(i -> data());
						glVertex4fv((i + 1) -> data());
						glVertex4fv((i + 2) -> data());
					}

				glEnd();

			glPopMatrix();
		}
		
		void drawShadowVolume(const Vector4& lightPosition) const
		{
			assert(lightPosition[w] == 1.0);

			//Get the simulated results back and update the model's orientation
			updateLocation();
		
			if(hasMoved || previousLightPosition != lightPosition)
			{
				//Color for debugging purposes
				glColor4f(0.0, 1.0, 0.0, 0.5);
				computeShadowVolume(lightPosition);
			}
			else
				glColor4f(1.0, 0.0, 0.0, 0.5);

			//Draw the cached shadow volume

			vector<Edge>::iterator startVertexPosition = edges.begin();
			vector<Vector4>::iterator startShadowVolumeVertexPosition = shadowVolumeVertices.begin();
			
			glMatrixMode(GL_MODELVIEW);
			glBegin(GL_QUAD_STRIP);
	
				vector<Edge>::iterator i;
				vector<Vector4>::iterator j;
				
				for(i = edges.begin(), j = shadowVolumeVertices.begin(); i != edges.end() && j != shadowVolumeVertices.end(); i++, j++)
				{
					glVertex4fv(i -> first.data());
					glVertex4fv(j -> data());
	
					if(i != edges.end() - 1 && i -> second != (i + 1) -> first)
					{
						glVertex4fv(startVertexPosition -> first.data());
						glVertex4fv(startShadowVolumeVertexPosition -> data());
	
						startVertexPosition = i + 1;
						startShadowVolumeVertexPosition = j + 1;
						
						glEnd();
						glBegin(GL_QUAD_STRIP);
					}
				}
	
				glVertex4fv(startVertexPosition -> first.data());
				glVertex4fv(startShadowVolumeVertexPosition -> data());
	
			glEnd();
		}

		vector<GLfloat>::size_type getVertexCount() const { return vertices.size(); }

		const Vector4& getColor() const { return color; }
		
		const Matrix44& getOrientation() const { return globalOrientation; }

		const NxActor& getActor() const { return actor; }

		void setColor(const Vector4& color) { this -> color = color; }

		void setKinematic(bool kinematic)
		{
			if(kinematic)
				actor.raiseBodyFlag(NX_BF_KINEMATIC);
			else
			{
				actor.clearBodyFlag(NX_BF_KINEMATIC);
				actor.wakeUp();	
			}
		}

		void applyForce(const NxVec3& forceDirection, const NxReal forceStrength)
		{
			NxVec3 forceVector = forceStrength * forceDirection;

			actor.addForce(forceVector);
			actor.setLinearVelocity(forceVector);
		}

		friend void operator >> (const char* filePath, SimulatedModel& model)
		{
			vector<GLfloat>::size_type vertexCount;
			ifstream in(filePath);
			
			if(in.fail())
				throw runtime_error("The specified file '" + string(filePath) + "' does not exist.");

			in >> vertexCount;

			assert(vertexCount % 3 == 0);

			model.vertices.clear();
			model.normals.clear();
			model.shadowVolumeVertices.clear();
			model.vertices.reserve(vertexCount);
			model.normals.reserve(vertexCount / 3);
			model.shadowVolumeVertices.reserve(vertexCount);
 
 			for(vector<Vector4>::size_type i = 0; i < vertexCount; i++)
 			{
 				//NOTE: Can we shorten this?
				Vector4 temp;
				
				in >> temp[x];
				in >> temp[y];
				in >> temp[z];
				in >> temp[w];
				
				model.vertices.push_back(temp);
 			}

			model.computeNormals();
		}
		
		friend void operator >> (const vector<Vector4>& vertices, SimulatedModel& model)
		{
			assert(vertices.size() % 3 == 0);

			model.vertices.clear();
			model.normals.clear();
			model.shadowVolumeVertices.clear();
			model.vertices.reserve(vertices.size());
			model.normals.reserve(vertices.size() / 3);
			model.shadowVolumeVertices.reserve(vertices.size());
 
 			model.vertices = vertices;
			model.computeNormals();
		}

		/*
		friend ostream& operator << (ostream& out, const SimulatedModel& model)
		{
			out << "Vertices:\n" << endl;

			for(vector<Vector4>::const_iterator i = model.vertices.begin(); i != model.vertices.end(); i++)
				out << *i << "\n";

			out << "\nNormals:\n" << endl;
			
			for(vector<Vector4>::const_iterator i = model.normals.begin(); i != model.normals.end(); i++)
			{
				out << *i;
				
				if(i != model.normals.end() - 1)
					out << "\n";
			}
				
			return out << flush;
		}
		*/

	private:
		void updateLocation() const
		{
			NxMat34 physicalOrientation;
			Matrix44 temp;

			physicalOrientation = actor.getGlobalPose();
			physicalOrientation.getColumnMajor44(temp);

			for(Matrix44::iterator i = globalOrientation.begin(), j = temp.begin(); i != globalOrientation.end() && j != temp.end(); i++, j++)
			{
				//We don't want to redraw the shadow volume if the simulation didn't move the model very much.
				if(*i != *j && abs(*i - *j) > moveThreshold)
					hasMoved = true;

				*i = *j;
			}
		}

		void computeShadowVolume(const Vector4& lightPosition) const
		{
			assert(lightPosition[w] == 1.0);

			if(hasMoved)
			{
				transformedVertices.clear();
				transformedVertices = vertices;
	
				for(vector<Vector4>::iterator i = transformedVertices.begin(); i != transformedVertices.end(); i++)
					*i = globalOrientation.getTranspose() * *i;
					//*i = globalOrientation * *i;
			}
	
			hasMoved = false;
			previousLightPosition = lightPosition;
			shadowVolumeVertices.clear();
			shadowVolumeVertices.reserve(vertices.size());
			edges.clear();
			edges.reserve(vertices.size());

			//For all triangles...
			for(vector<Vector4>::iterator i = transformedVertices.begin(); i != transformedVertices.end(); i += 3)
			{
				//If the triangle is facing the light...
				if(Vector4::dotProduct(computeNormal(*i, *(i + 1), *(i + 2)), lightPosition) > 0.0)
				{
					vector<Edge> triangle;
					
					triangle.push_back(Edge(*i      , *(i + 1)));
					triangle.push_back(Edge(*(i + 1), *(i + 2)));
					triangle.push_back(Edge(*(i + 2), *i));

					for(vector<Edge>::iterator i = triangle.begin(); i != triangle.end(); i++)
					{	
						bool found = false;
						vector<Edge>::iterator removeLocation;
						
						for(vector<Edge>::iterator j = edges.begin(); j != edges.end() && !found; j++)
							if(*i == *j || *i == Edge(j -> second, j -> first))
							{
								found = true;
								removeLocation = remove(edges.begin(), edges.end(), *j);
							}
						
						//If a duplicate edge exists, we remove all instances of the edge from our container.
						if(found)
							edges.erase(removeLocation, edges.end());
						//Otherwise, it is a candidate for a silhouette edge.
						else
							edges.push_back(*i);
					}
				}
			}
			
			//We must sort the edges to use a quad strip.
			sortEdges(edges);

			for(vector<Edge>::iterator i = edges.begin(); i != edges.end(); i++)
			{
				//Extrude the vertex to infinity
				shadowVolumeVertices.push_back((i -> first - lightPosition).normalize());

				//Vertex is at infinity
				assert(shadowVolumeVertices.back()[w] == 0.0);
			}
		}

		void computeNormals() const
		{
			for(vector<Vector4>::const_iterator i = vertices.begin(); i != vertices.end(); i += 3)
				normals.push_back(computeNormal(*i, *(i + 1), *(i + 2)));
		}
		
		static Vector4 computeNormal(const Vector4& vertex1, const Vector4& vertex2, const Vector4& vertex3)
		{
			assert(vertex1[w] == 1.0 && vertex2[w] == 1.0 && vertex3[w] == 1.0);
			
			Vector4 vector4(vertex2[x] - vertex1[x], vertex2[y] - vertex1[y], vertex2[z] - vertex1[z], 0.0);
			Vector4 vector5(vertex3[x] - vertex1[x], vertex3[y] - vertex1[y], vertex3[z] - vertex1[z], 0.0);
			Vector4 normal = Vector4::crossProduct(vector4, vector5);
		
			normal.normalize();
			
			return normal;
		}
		
		static void sortEdges(vector<Edge>& edges)
		{
			bool swapped;
			
			for(vector<Edge>::iterator i = edges.begin(); i != edges.end() - 1; i++)
			{
				swapped = false;
				
				for(vector<Edge>::iterator j = i + 1; j != edges.end() && !swapped; j++)
					if(i -> second == j -> first || i -> second == j -> second)
					{
						if(i -> second == j -> second)
							swap(j -> first, j -> second);
							
						swap(*(i + 1), *j);
						
						swapped = true;
					}
			}
		}
};

const GLfloat SimulatedModel::moveThreshold = 0.08;

#endif /*SIMULATEDMODEL_H_*/