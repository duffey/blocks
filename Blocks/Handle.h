#ifndef HANDLE_H_
#define HANDLE_H_

#include <algorithm> // swap
#include <cassert>   // assert
#include <iostream>  // cout, endl

/**
  * @brief This class is a wrapper for a pointer.
  * Among other things, it allows us to have containers of non-homogeneous objects that inherit from the same parent class.
  */
template <typename T>
class Handle
{ 
    public:
        typedef T                 value_type;

        typedef value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef value_type&       reference;
        typedef const value_type& const_reference;

    private:
        pointer p;

    public:
    	Handle () : p(0) {}
    
        Handle (pointer p) : p(p) {}

        Handle (const Handle& other) : p(other.p -> clone()) { assert(p); }

        ~Handle () { delete p; }

        Handle& operator = (Handle other)
		{
            swap(other);

            return *this;
		}

        reference operator * () { return *p; }

        const_reference operator * () const { return *p; }

        pointer operator -> () { return p; }

        const_pointer operator -> () const { return p; }

		void swap (Handle& other) { std::swap(p, other.p); }

		friend bool operator == (const Handle& lhs, const Handle& rhs)
		{
    		return lhs.p == rhs.p;
		}

		friend bool operator != (const Handle& lhs, const Handle& rhs)
		{
    		return !(lhs.p == rhs.p);
		}
};
            
#endif /*HANDLE_H_*/
