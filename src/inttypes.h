#ifndef TYPES_H
#define TYPES_H

namespace internal {
	class ERROR_inttype_not_available
	{
		ERROR_inttype_not_available();
	};

	template<bool FIRST, class A, class B> 
	struct select
	{
		typedef A T;
	};

	template<class A, class B> 
	struct select<false, A, B>
	{
		typedef B T;
	};
	
}	// namespace internal


// Retrieves a signed/unsigned integer type with sizeof(T) == BYTES
template<unsigned BYTES, bool SIGNED>
struct int_t
{
	typedef typename internal::select<sizeof(signed char)       == BYTES, signed char,
			  typename internal::select<sizeof(signed short)      == BYTES, signed short,
			  typename internal::select<sizeof(signed int)        == BYTES, signed int,
			  typename internal::select<sizeof(signed long)       == BYTES, signed long,
			  typename internal::select<sizeof(signed long long)  == BYTES, signed long long,
			  internal::ERROR_inttype_not_available>::T>::T>::T>::T>::T T;
};

template<unsigned BYTES>
struct int_t<BYTES, false>
{
	typedef typename internal::select<sizeof(unsigned char)      == BYTES, unsigned char,
		     typename internal::select<sizeof(unsigned short)     == BYTES, unsigned short,
			  typename internal::select<sizeof(unsigned int)       == BYTES, unsigned int,
			  typename internal::select<sizeof(unsigned long)      == BYTES, unsigned long,
			  typename internal::select<sizeof(unsigned long long) == BYTES, unsigned long long,
			  internal::ERROR_inttype_not_available>::T>::T>::T>::T>::T T;
};

// Retrieves the smallest unsigned integer type with sizeof(T) >= BYTES
template<unsigned BYTES>
struct uint_t_min
{
	typedef typename internal::select<sizeof(unsigned char)      >= BYTES, unsigned char,
		     typename internal::select<sizeof(unsigned short)     >= BYTES, unsigned short,
			  typename internal::select<sizeof(unsigned int)       >= BYTES, unsigned int,
			  typename internal::select<sizeof(unsigned long)      >= BYTES, unsigned long,
			  typename internal::select<sizeof(unsigned long long) >= BYTES, unsigned long long,
			  internal::ERROR_inttype_not_available>::T>::T>::T>::T>::T T;
};


// Machine independent definition of sized integer types
typedef int_t<1, true>::T	Int8;
typedef int_t<2, true>::T	Int16;
typedef int_t<4, true>::T	Int32;
typedef int_t<8, true>::T	Int64;
typedef int_t<1, false>::T	UInt8;
typedef int_t<2, false>::T	UInt16;
typedef int_t<4, false>::T	UInt32;
typedef int_t<8, false>::T	UInt64;

#endif
