/*	Introspection.cpp :
	This metacode, illustrating compile time introspection, is lifted wholesale from https://jguegant.github.io/blogs/tech/sfinae-introduction.html
	And is the elaborate and ingenious creation of Jean Guegant, who used template code to add a much needed feature to a language that did not possess it.
*/

#include <iostream>
#include <type_traits>
#include <string>

template <class T> struct has_serialize
{
	// We test if the type has serialize using decltype and declval.
	// This is a templated struct with a templated method test, which is ingeniously used to give it one static member, whether class T has the method of the desired type.
	//	Class T is substituted for class U in the final construction of its one and only member.
	//	Which static and constexpr member is called "value", hence that weird piece of syntax sugar.
	//	Which unintellible piece of syntax sugar exists because "has_serialize<T>::value"  is cleaner than "has_serialize<T>::test<T>(int())"
	template <typename U> static constexpr decltype(std::declval<U>().serialize(), bool()) test(int)
	{
		// We can return values, thanks to constexpr instead of playing with sizeof.
		return true;
	}

	template <typename U> static constexpr bool test(...)
	{
		return false;
	}
	// int is used to give the precedence!
	static constexpr bool value = has_serialize:: template test<T>(int());  //Has to be static, has to be constexpr, and has to have a name, for arcane and complicated syntax sugar reasons.
	//	The has_serialize:: template is unnecessary syntax sugar in this context and can be deleted, because membership is assumed, and test is explicitly a template function, but why give the compiler a choice?  In template code, compiler choices are apt to be mysterious and unexpected.
	//	The template keyword, when it follows a "::", a "." or a "->", indicates use of a template, not declaration of a template, but in other contexts indicates declaration of a template, generating a compile time error.
	//	You might need the template keyword when a templated member is invoked by overload, though it is not needed here.
	//	But, depending on the compiler, you may need the template declaration when you invoke a template member of a template.  Which you do a lot of in metacode.
};

template <class T> struct has_to_string
{
	// We test if the type has serialize using decltype and declval.
	// This is a templated struct with a templated method test, which is ingeniously used to give it one static member, whether class T has the method of the desired type.
	//	Class T is substituted for class U in the final construction of its one and only member.
	//	Which static and constexpr member is called "value", hence that weird piece of syntax sugar.
	//	Which unintellible piece of syntax sugar exists because "has_serialize<T>::value"  is cleaner than "has_serialize<T>::test<T>(int())"
	template <typename U> static constexpr decltype(to_string(std::declval<U>()), bool()) test(int)
	{
		// We can return values, thanks to constexpr instead of playing with sizeof.
		return true;
	}

	template <typename U> static constexpr bool test(...)
	{
		return false;
	}
	// int is used to give the precedence!
	static constexpr bool value = has_to_string::template test<T>(int());  //Has to be static, has to be constexpr, and has to have a name, for arcane and complicated syntax sugar reasons.  The has_to_string::template is redundant, because the compiler can figure that out on its own, but it is often surprising what the compiler fails to figure out.
};

//	Simple notation is "template <typename T> typename std::enable_if<has_serialize<T>::value, std::string>::type serialize(T & obj)"
//	Can use the complicated notation has_serialize<T>::test<T>(int()) to reveal the underlying syntax sugar.
template <typename T> typename std::enable_if<has_serialize<T>::value, std::string>::type serialize(T & obj)
		//	value is the arbitrary name we gave to the return member of the class has_serialize<T>
		//	type is the arbitrary name the stl gave to the return member of the class std::enable_if<bool, class>
		//	"typename" forces the compiler to interpret the declaration as a declaration.
		//	Within templates, the compiler is apt to randomly interpret "class" as in unexpected ways for reasons unintellible to mortals.
		//	albeit with template template parameters, one sometimes has to us "class" and not "typename" for reasons equally incomprehensible.
		//	and different compilers, and different updates of the compiler, behave differently and unpredictably.
		//	but usually the problem inside templates is "class" failing strangely, and "typename" working for reasons entirely opaque.
		//	typename tells the compiler explicitly that typename Something<ME, u>::Result is actually a type and not just a variable
	{
	return obj.serialize();
}

template <typename T> typename std::enable_if_t<std::is_arithmetic<T>::value, std::string> serialize(T obj)
{
		return std::string("Numeric value ")+ std::to_string(obj);
}

template <typename T> typename std::enable_if_t<has_to_string<T>::value && !has_serialize<T>::value && !std::is_integral<T>::value, std::string>serialize(T & obj)
{
	return to_string(obj);
}

template < typename T,
	typename std::enable_if< std::is_integral< T >::value, T >::type* dummy_arg = nullptr >
	std::string serialize2(T obj) {
	return std::string("Integral value ") + std::to_string(obj);
}
template < typename T,
	typename std::enable_if< std::is_floating_point< T >::value, T >::type* dummy_arg = nullptr >
	std::string serialize2(T obj) {
	return std::string("floating point value ") + std::to_string(obj);
}
template < typename T,
	typename std::enable_if< has_serialize<T>::value, T >::type* dummy_arg = nullptr >
	std::string serialize2(T &obj) {
	return std::string("this type has member or functor serialize(), which reports: ") + obj.serialize();
}
template < typename T,
	typename std::enable_if< !std::is_arithmetic< T >::value && !has_serialize<T>::value, T >::type* dummy_arg = nullptr >
	std::string serialize2(T obj) {
	return std::string("not a recognized type");
}

template <typename T>  std::string serialize_by_argtype(typename std::enable_if_t<std::is_integral<T>::value, T>  obj) {
	return std::string("Integral value ") + std::to_string(obj);
}

template <typename T>  std::string serialize_by_argtype(typename std::enable_if_t<std::is_arithmetic<T>::value&&!std::is_integral<T>::value, T>  obj) {
	return std::string("Numeric value ") + std::to_string(obj);
}
template <typename T>  std::string serialize_by_argtype(typename std::enable_if_t<has_to_string<T>::value, T>  obj) {
	return to_string(obj);
}
template <typename T>  std::string serialize_by_argtype(typename std::enable_if_t< !std::is_arithmetic< T >::value && !has_to_string<T>::value, T>  obj){
	return std::string("not a recognized type");
}
template <typename T>  std::string serialize3(T  obj){
	return serialize_by_argtype<decltype(obj)>(obj);
	// Need this indirection to tell the compiler how to choose the type of the template.
}


//Declare a bunch of trivial classes to test out compile time type introspection for methods.

struct A {};

std::string to_string(const A&)
{
	return u8R"*(I am a A, and the "std::string to_string(const A&)" function is explicitly defined to my type )*";
}

// Type B with a serialize method.
struct B
{
	std::string serialize() const
	{
		return u8R"(I am a B, and the "std::string serialize() const" method is a member of my type )";
	}
};

// Type C with a "wrong" serialize member (not a method) and a to_string overload.
struct C
{
	std::string serialize;
};

std::string to_string(const C&)
{
	return u8R"(I am a C, which has the wrong serialize function defined, but the to_string outside function defined.)";
}

struct D : A
{
	std::string serialize() const
	{
		return u8R"(I am a D, and also have the "std::string serialize() const" method defined)";
	}
};

struct E
{
	struct _X
	{
		std::string operator()() {
			return "I am a E, and have a member functor serialize instead of method serialize to return this string";
		}
	};
	_X serialize;
};


int main() {
	A a;
	B b;
	C c;
	D d;
	E e;
	int i{ 5 };
	std::cout << u8R"(
The templated function "serialize" is called with an argument belonging to a wide variety of classes, 
and uses the correct member or method of each class, selecting the correct template by generating 
an invalid return type if the template is instantiated for a class with the wrong properties.
)";

	std::cout << serialize(a) << std::endl;
	std::cout << serialize(b) << std::endl;
	std::cout << serialize(c) << std::endl;
	std::cout << serialize(d) << std::endl;
	std::cout << serialize(e) << std::endl;
	std::cout << serialize(i) << std::endl;
	std::cout << serialize(7) << std::endl;
	std::cout << serialize(7.7) << std::endl;
	std::cout << u8R"(
The templated function "serialize2" is called with an argument belonging to a wide variety of classes, 
and uses the correct member or method of each class, selecting the correct template by template 
parameter, by generating an invalid template parameter if the template is instantiated for a class
with the wrong properties.
)";
	std::cout << serialize2(a) << std::endl;
	std::cout << serialize2(b) << std::endl;
	std::cout << serialize2(c) << std::endl;
	std::cout << serialize2(d) << std::endl;
	std::cout << serialize2(e) << std::endl;
	std::cout << serialize2(i) << std::endl;
	std::cout << serialize2(7) << std::endl;
	std::cout << serialize2(7.7) << std::endl;

	std::cout << u8R"(
The templated function "serialize3" is called with an argument belonging to a wide variety of classes, 
and uses the correct member or method of each class, selecting the correct template by argument type, 
by generating an invalid argument type if the template is instantiated for a class
with the wrong properties.

It uses indirection to inform the compiler not to be too clever at figuring out the class, which 
produces an extra level of runtime calling, which one hopes the compiler optimizes away.

)";
	float y{ 9.9 };
	std::cout << serialize_by_argtype<decltype(y)>(y) << std::endl;  //The compiler cannot figure out which type to use when one uses enquire_if_t on the argument type, rather than the return value.
	std::cout << serialize3(a) << std::endl;
	std::cout << serialize3(b) << std::endl;
	std::cout << serialize3(c) << std::endl;
	std::cout << serialize3(d) << std::endl;
	std::cout << serialize3(e) << std::endl;
	std::cout << serialize3(i) << std::endl;
	std::cout << serialize3(7) << std::endl;
	std::cout << serialize3(7.7) << std::endl;
	return 0;
}
