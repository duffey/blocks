#ifndef MATRIX44_H_
#define MATRIX44_H_

#include <cstddef>		//size_t
#include <iostream>		//ostream
#include <algorithm>	//fill and copy
#include <cassert>		//assert

#include "Vector4.h"

using namespace std;

/**
  * @brief This class defines a container and operations for manipulating matrices.
  */
class Matrix44
{
	//NOTE: ADD data() METHOD AND REMOVE CONVERSION OPERATOR
	public:
		typedef GLfloat value_type;
	    typedef GLfloat& reference;
	    typedef const GLfloat& const_reference;
	    typedef GLfloat* iterator;
	    typedef const GLfloat* const_iterator;
		typedef size_t size_type;

		static const size_type static_size = 16;
	
	private:
		GLfloat matrix[4][4];

	public:
		Matrix44()
		{
			//Identity matrix
			for(size_t i = 0; i < 4; i++)
				for(size_t j = 0; j < 4; j++)
					matrix[i][j] = (i == j);
		}

		Matrix44(GLfloat initialValue)
		{
			for(size_t i = 0; i < 4; i++)
				for(size_t j = 0; j < 4; j++)
					matrix[i][j] = initialValue;
		}
		
		Matrix44(GLfloat* matrix)
		{
			for(size_t i = 0; i < static_size; i++)
				this -> matrix[i / 4][i % 4] = matrix[i];
		}
		
		Matrix44(const Matrix44& other)
		{
			for(size_t i = 0; i < 4; i++)
				for(size_t j = 0; j < 4; j++)
					matrix[i][j] = other.matrix[i][j];
		}

		Matrix44 getTranspose() const
		{
			Matrix44 transpose;
			
			for(size_t i = 0; i < 4; i++)
				for(size_t j = 0; j < 4; j++)
					transpose[j][i] = matrix[i][j];
					
			return transpose;
		}

		size_type size() const { return static_size; }

		iterator begin() { return &matrix[0][0]; }
		
		iterator end() { return &matrix[0][0] + size(); }

		const_iterator begin() const { return &matrix[0][0]; }
		
		const_iterator end() const { return &matrix[0][0] + size(); }

		GLfloat* operator [] (const size_t i) { return matrix[i]; }

		const GLfloat* operator [] (const size_t i) const { return matrix[i]; }
		
		Matrix44& operator = (const Matrix44& other)
		{
			copy(other.begin(), other.end(), begin());
			
			return *this;
		}
		
		const Vector4 operator * (const Vector4& vector) const
		{
			Vector4 product(0.0, 0.0, 0.0, 0.0);
			
			for(size_t i = 0; i < 4; i++)
				for(size_t j = 0; j < 4; j++)
					product[i] += vector[j] * matrix[i][j];
			
			return product;
		}

		friend ostream& operator << (ostream& lhs, const Matrix44& rhs)
		{
			for(size_t i = 0; i < 4; i++)
			{
				for(size_t j = 0; j < 4; j++)
				{
					lhs << rhs.matrix[i][j];

					if(j < 3)
						lhs << " ";
				}

				if(i < 3)
					lhs << endl;
			}

			return lhs;
		}

		//value_type* data() { return vector; }
		
		//const value_type* data() const { return vector; }

		operator GLfloat* () { return &matrix[0][0]; }
		
		operator const GLfloat* () const { return &matrix[0][0]; }
};

#endif /*MATRIX44_H_*/
