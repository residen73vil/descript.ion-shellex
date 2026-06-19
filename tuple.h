// File: msvc_tuple.h
// Implements tuple3 for older compilers.
#ifndef MSVC_TUPLE_H
#define MSVC_TUPLE_H

// Simple 3-element tuple for MSVC 6.0
template<class T1, class T2, class T3>
class msvc_tuple {
public:
	T1 first;
	T2 second;
	T3 third;
	
	// Default constructor
	msvc_tuple() {}
	
	// Constructor with values
	msvc_tuple(const T1& a, const T2& b, const T3& c)
		: first(a), second(b), third(c) {}
};

// Helper function - simpler version
template<class T1, class T2, class T3>
inline msvc_tuple<T1, T2, T3> make_msvc_tuple(const T1& a, const T2& b, const T3& c) {
	return msvc_tuple<T1, T2, T3>(a, b, c);
}

// Simple get functions (no templates on return type)
template<class T1, class T2, class T3>
inline T1& get0(msvc_tuple<T1, T2, T3>& t) { return t.first; }

template<class T1, class T2, class T3>
inline const T1& get0(const msvc_tuple<T1, T2, T3>& t) { return t.first; }

template<class T1, class T2, class T3>
inline T2& get1(msvc_tuple<T1, T2, T3>& t) { return t.second; }

template<class T1, class T2, class T3>
inline const T2& get1(const msvc_tuple<T1, T2, T3>& t) { return t.second; }

template<class T1, class T2, class T3>
inline T3& get2(msvc_tuple<T1, T2, T3>& t) { return t.third; }

template<class T1, class T2, class T3>
inline const T3& get2(const msvc_tuple<T1, T2, T3>& t) { return t.third; }

#endif // MSVC_TUPLE_H
