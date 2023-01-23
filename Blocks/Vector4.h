#ifndef VECTOR4_H_
#define VECTOR4_H_

#include <cstddef>		//size_t
#include <algorithm>	//fill, and copy
#include <cassert>		//assert
#include <cmath>		//sqrt

using namespace std;

/**
  * @brief This class defines a container and operations for manipulating vectors.
  */
class Vector4
{
	public:
		typedef GLfloat value_type;
	    typedef GLfloat& reference;
	    typedef const GLfloat& const_reference;
	    typedef GLfloat* iterator;
	    typedef const GLfloat* const_iterator;
		typedef size_t size_type;

		static const size_type static_size = 4;
		
	private:
		enum { x, y, z, w };
		GLfloat vector[4];

	public:
		Vector4() { fill(begin(), end(), 0.0); }
		
		Vector4(value_type component1, value_type component2, value_type component3, value_type component4)
		{
			vector[x] = component1;
			vector[y] = component2;
			vector[z] = component3;
			vector[w] = component4;
		}

		Vector4(const Vector4& other) { copy(other.begin(), other.end(), begin()); }

		size_type size() const { return static_size; }
		
		iterator begin() { return vector; }
		
		iterator end() { return vector + size(); }

		const_iterator begin() const { return vector; }
		
		const_iterator end() const { return vector + size(); }
		
		reference operator [] (const size_type i) { return vector[i]; }

		const_reference operator [] (const size_type i) const { return vector[i]; }

		/*
		friend ostream& operator << (ostream& lhs, const Vector4& rhs)
		{
			for(Vector4::const_iterator i = rhs.begin(); i != rhs.end(); i++)
			{
				lhs << *i;
				
				if(i != rhs.end() - 1)
					lhs << " ";
			}

			return lhs;
		}
		*/

		Vector4& operator = (const Vector4& other)
		{
			copy(other.begin(), other.end(), begin());
			
			return *this;
		}
		
		value_type* data() { return vector; }
		
		const value_type* data() const { return vector; }
		
		value_type magnitude() const { return sqrt(pow(vector[x], 2) + pow(vector[y], 2) + pow(vector[z], 2) + pow(vector[w], 2)); }
		
		Vector4& normalize()
		{
			GLfloat m = magnitude();
			
			for(iterator i = begin(); i != end(); i++)
				*i /= m;
				
			return *this;
		}
		
		static value_type dotProduct(const Vector4& vector1, const Vector4& vector2)
		{
			return vector1[x] * vector2[x] + vector1[y] * vector2[y] + vector1[z] * vector2[z] + vector1[w] * vector2[w];
		}
		
		static Vector4 crossProduct(const Vector4& vector1, const Vector4& vector2)
		{
			assert(vector1[w] == 0.0 && vector2[w] == 0.0);
							   
			return Vector4(	vector1[y] * vector2[z] - vector1[z] * vector2[y],
							vector1[z] * vector2[x] - vector1[x] * vector2[z],
							vector1[x] * vector2[y] - vector1[y] * vector2[x],
							0.0);
		}
		
		friend Vector4 operator + (const Vector4& lhs, const Vector4& rhs)
		{
			return Vector4(	lhs[x] + rhs[x],
							lhs[y] + rhs[y],
							lhs[z] + rhs[z],
							lhs[w] + rhs[w]);
		}
		
		friend Vector4 operator - (const Vector4& lhs, const Vector4& rhs)
		{
			return Vector4(	lhs[x] - rhs[x],
							lhs[y] - rhs[y],
							lhs[z] - rhs[z],
							lhs[w] - rhs[w]);
		}
		
		friend Vector4 operator * (const Vector4& vector, const value_type scalar)
		{
			return Vector4(	vector[x] * scalar,
							vector[y] * scalar,
							vector[z] * scalar,
							vector[w] * scalar);
		}
		
		friend Vector4 operator * (const value_type scalar, const Vector4& vector) { return vector * scalar; }
		
		friend bool operator == (const Vector4& lhs, const Vector4& rhs)
		{
			return equal(lhs.begin(), lhs.end(), rhs.begin());
		}
		
		friend bool operator != (const Vector4& lhs, const Vector4& rhs)
		{
			return !(lhs == rhs);
		}
};

#endif /*VECTOR4_H_*/
