/*	Introspection.cpp :
	This metacode, illustrating compile time introspection, is lifted wholesale from https://jguegant.github.io/blogs/tech/sfinae-introduction.html
	And is the elaborate and ingenious creation of Jean Guegant, who used template code to add a much needed feature to a language that did not possess it.
*/

#include <iostream>
#include <type_traits>
#include <string>


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

template <class T> struct hasSerialize
{
	// We test if the type has serialize using decltype and declval.
	// This is a templated struct with a templated method test, which is ingeniously used to give it one static member, whether class T has the method of the desired type.
	//	Class T is substituted for class U in the final construction of its one and only member.
	//	Which static and constexpr member is called "value", hence that weird piece of syntax sugar.
	//	Which unintellible piece of syntax sugar exists because "hasSerialize<T>::value"  is cleaner than "hasSerialize<T>::test<T>(int())"
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
	static constexpr bool value = hasSerialize:: template test<T>(int());  //Has to be static, has to be constexpr, and has to have a name, for arcane and complicated syntax sugar reasons.
	//	The hasSerialize:: template is unnecessary syntax sugar in this context and can be deleted, because membership is assumed, and test is explicitly a template function, but why give the compiler a choice?  In template code, compiler choices are apt to be mysterious and unexpected.
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
	//	Which unintellible piece of syntax sugar exists because "hasSerialize<T>::value"  is cleaner than "hasSerialize<T>::test<T>(int())"
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
	static constexpr bool value = test<T>(int());  //Has to be static, has to be constexpr, and has to have a name, for arcane and complicated syntax sugar reasons.
};

//	Simple notation is "template <typename T> typename std::enable_if<hasSerialize<T>::value, std::string>::type serialize(T & obj)"
//	Can use the complicated notation hasSerialize<T>::test<T>(int()) to reveal the underlying syntax sugar.
template <typename T> typename std::enable_if<hasSerialize<T>::value, std::string>::type serialize(T & obj)
		//	value is the arbitrary name we gave to the return member of the class hasSerialize<T>
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

template <typename T> typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type serialize(T obj)
{
		return std::string("Numeric value ")+ std::to_string(obj);
}

template <typename T> typename std::enable_if<has_to_string<T>::value && !hasSerialize<T>::value && !std::is_integral<T>::value, std::string>::type serialize(T & obj)
{
	return to_string(obj);
}

int main() {
	A a;
	B b;
	C c;
	D d;
	E e;
	int i{ 5 };


	std::cout << u8R"(The templated function "serialize" is called with an argument belonging to a wide variety of classes, and uses the correct member of each class

)";

	std::cout << serialize(a) << std::endl;
	std::cout << serialize(b) << std::endl;
	std::cout << serialize(c) << std::endl;
	std::cout << serialize(d) << std::endl;
	std::cout << serialize(e) << std::endl;	
	std::cout << serialize(i) << std::endl;
	std::cout << serialize(7) << std::endl;	
	std::cout << serialize(7.7) << std::endl;
	return 0;
}