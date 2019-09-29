# Introspection in C++11
Compile time introspection in C++11 - The function has three different templates, according to the properties of the type

This is a toy example program, does not do anything useful, but it shows how to examine the type on which the template is invoked, and to provide one template for a type with one sort of member, another template if it has a different kind of member, and no template at all if the type will not work.

It generates the following output:<pre>The templated function "serialize" is called with an argument belonging to a wide variety of classes,
and uses the correct member or method of each class, selecting the correct template by generating
an invalid return type if the template is instantiated for a class with the wrong properties.

I am a A, and the "std::string to_string(const A&)" function is explicitly defined to my type
I am a B, and the "std::string serialize() const" method is a member of my type
I am a C, which has the wrong serialize function defined, but the to_string outside function defined.
I am a D, and also have the "std::string serialize() const" method defined
I am a E, and have a member functor serialize instead of method serialize to return this string
Numeric value 5
Numeric value 7
Numeric value 7.700000

The templated function "serialize2" is called with an argument belonging to a wide variety of classes,
and uses the correct member or method of each class, selecting the correct template by template
parameter, by generating an invalid template parameter if the template is instantiated for a class
with the wrong properties.

not a recognized type
this type has member or functor serialize(), which reports: I am a B, and the "std::string serialize() const" method is a member of my type
not a recognized type
this type has member or functor serialize(), which reports: I am a D, and also have the "std::string serialize() const" method defined
this type has member or functor serialize(), which reports: I am a E, and have a member functor serialize instead of method serialize to return this string
Integral value 5
Integral value 7
floating point value 7.700000

The templated function "serialize3" is called with an argument belonging to a wide variety of classes,
and uses the correct member or method of each class, selecting the correct template by argument type,
by generating an invalid argument type if the template is instantiated for a class
with the wrong properties.

It uses indirection to inform the compiler not to be too clever at figuring out the class, which
produces an extra level of runtime calling, which one hopes the compiler optimizes away.

Numeric value 9.900000
I am a A, and the "std::string to_string(const A&)" function is explicitly defined to my type
not a recognized type
I am a C, which has the wrong serialize function defined, but the to_string outside function defined.
I am a A, and the "std::string to_string(const A&)" function is explicitly defined to my type
not a recognized type
Integral value 5
Integral value 7
Numeric value 7.700000</pre>


