/*- cnTK - cpp feature testing --------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-09-05                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_CPPFeatureCheck_H__
#define	__cnLibrary_cnTK_CPPFeatureCheck_H__
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
// __cplusplus
// c++98/03		199711L
// c++11		201103L
// c++14		201402L
// c++17		201703L
// c++20		202002L

//- C++11 -------------------------------------------------------------------

// (C++11)200907L __cpp_lambdas						Lambda expressions
// (C++11)200907L __cpp_range_based_for				Range-based for loop
// (C++11)200710L __cpp_raw_strings					Raw string literals
// (C++11)200806L __cpp_threadsafe_static_init		Dynamic initialization and destruction with concurrency
// (C++11)200710L __cpp_unicode_literals			Unicode string literals
// (C++11)200809L __has_cpp_attribute(carries_dependency)	[[carries_dependency]]
// (C++11)200809L __has_cpp_attribute(noreturn)				[[noreturn]]

// (C++11)200809L __cpp_attributes					Attributes
#define	cnLibrary_CPPFEATURE_ATTRIBUTES				__cpp_attributes

// (C++11)200704L __cpp_unicode_characters			New character types (char16_t and char32_t)
#define	cnLibrary_CPPFEATURE_UCHARS					__cpp_unicode_characters

// (C++11)200907L __cpp_lambdas						Lambda expressions
#define	cnLibrary_CPPFEATURE_LAMBDAS				__cpp_lambdas


// (C++11)200704L __cpp_constexpr					constexpr
// (C++14)201304L __cpp_constexpr					Relaxed constexpr, non-const constexpr methods
// (C++17)201603L __cpp_constexpr					Constexpr lambda
// (C++20)201907L __cpp_constexpr					Trivial default initialization and asm-declaration in constexpr functions
// (C++20)202002L __cpp_constexpr					Changing the active member of a union in constant evaluation
// (C++23)202110L __cpp_constexpr					Non-literal variables, labels, and goto statements in constexpr functions
// (C++23)202207L __cpp_constexpr					Relaxing some restrictions on constexpr functions and function templates
// (C++23)202211L __cpp_constexpr					Permitting static constexpr variables in constexpr functions
#define	cnLibrary_CPPFEATURE_CONSTEXPR				__cpp_constexpr

// (C++11)200410L __cpp_static_assert				static_assert
// (C++17)201411L __cpp_static_assert				Single-argument static_assert
#define	cnLibrary_CPPFEATURE_STATIC_ASSERT			__cpp_static_assert


// (C++11)200704L __cpp_alias_templates				Alias templates
#define	cnLibrary_CPPFEATURE_ALIAS_TEMPLATES		__cpp_alias_templates


// (C++11)200707L __cpp_decltype					decltype
#define	cnLibrary_CPPFEATURE_DECLTYPE				__cpp_decltype


// (C++11)200604L __cpp_delegating_constructors		Delegating constructors
#define	cnLibrary_CPPFEATURE_DELEGATE_CONSTRUCTORS	__cpp_delegating_constructors


// (C++11)200802L __cpp_inheriting_constructors		Inheriting constructors
// (C++11)201511L __cpp_inheriting_constructors		Rewording inheriting constructors
#define	cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS	__cpp_inheriting_constructors

// (C++11)200806L __cpp_initializer_lists			List-initialization and std::initializer_list
#define	cnLibrary_CPPFEATURE_INITIALIZER_LIST		__cpp_initializer_lists


// (C++11)200809L __cpp_nsdmi						Non-static data member initializers
#define	cnLibrary_CPPFEATURE_NSDMI					__cpp_nsdmi


// (C++11)200710L __cpp_ref_qualifiers				ref-qualifiers
#define	cnLibrary_CPPFEATURE_REF_QUALIFIERS			__cpp_ref_qualifiers


// (C++11)200610L __cpp_rvalue_references			Rvalue reference
#define	cnLibrary_CPPFEATURE_RVALUE_REFERENCES		__cpp_rvalue_references

// (C++11)200809L __cpp_user_defined_literals		User-defined literals
#define	cnLibrary_CPPFEATURE_USER_DEFINED_LITERALS	__cpp_user_defined_literals


// (C++11)200704L __cpp_variadic_templates			Variadic templates
#define	cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES		__cpp_variadic_templates


//- C++14 -------------------------------------------------------------------

// (C++14)201304L __cpp_aggregate_nsdmi				Aggregate classes with default member initializers
// (C++14)201304L __cpp_binary_literals				Binary literals
// (C++14)201304L __cpp_generic_lambdas				Generic lambda expressions
// (C++14)201304L __cpp_return_type_deduction		Return type deduction for normal functions
// (C++14)201309L __has_cpp_attribute(deprecated)			[[deprecated]]

// (C++14)201304L __cpp_init_captures				Lambda init-capture
// (C++20)201803L __cpp_init_captures				Allow pack expansion in lambda init-capture

// (C++14)201304L __cpp_decltype_auto				Return type deduction for normal functions
#define	cnLibrary_CPPFEATURE_DECLTYPE_AUTO			__cpp_decltype_auto

// (C++14)201309L __cpp_sized_deallocation			Sized deallocation
#define	cnLibrary_CPPFEATURE_SIZED_DEALLOCATION		__cpp_sized_deallocation

// (C++14)201304L __cpp_variable_templates			Variable templates
#define	cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES		__cpp_variable_templates

//- C++17 -------------------------------------------------------------------

// (C++17)201603L __cpp_aggregate_bases				Aggregate classes with base classes
// (C++17)201603L __cpp_capture_star_this			Lambda capture of *this by value as [=,*this]
// (C++17)201411L __cpp_enumerator_attributes		Attributes for enumerators
// (C++17)201603L __cpp_fold_expressions			Fold expressions
// (C++17)201606L __cpp_guaranteed_copy_elision		Guaranteed copy elision through simplified value categories
// (C++17)201603L __cpp_hex_float					Hexadecimal floating literals
// (C++17)201511L __cpp_inheriting_constructors		Rewording inheriting constructors
// (C++17)201411L __cpp_namespace_attributes		Attributes for namespaces
// (C++17)201411L __cpp_nontype_template_args		Allow constant evaluation for all non-type template arguments
// (C++17)201603L __cpp_range_based_for				Range-based for loop with different begin/end types
// (C++17)201606L __cpp_structured_bindings			Structured bindings
// (C++17)201611L __cpp_template_template_args		Matching of template template-arguments
// (C++17)201611L __cpp_variadic_using				Pack expansions in using-declarations
// (C++17)201603L __has_cpp_attribute(fallthrough)			[[fallthrough]]
// (C++17)201603L __has_cpp_attribute(maybe_unused)			[[maybe_unused]]
// (C++17)201603L __has_cpp_attribute(nodiscard)			[[nodiscard]]

// (C++17)201703L __cpp_deduction_guides			Template argument deduction for class templates

// (C++17)201606L __cpp_aligned_new					Dynamic memory allocation for over-aligned data
#define	cnLibrary_CPPFEATURE_ALIGNED_NEW		__cpp_aligned_new

// (C++17)201606L __cpp_inline_variables			Inline variables
#define	cnLibrary_CPPFEATURE_INLINE_VARIABLES	__cpp_inline_variables

// (C++17)201606L __cpp_if_constexpr				constexpr if
#define	cnLibrary_CPPFEATURE_IF_CONSTEXPR		__cpp_if_constexpr

// (C++17)201510L __cpp_noexcept_function_type		Make exception specifications be part of the type system
#define	cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE		__cpp_noexcept_function_type

// (C++17)201606L __cpp_nontype_template_parameter_auto		Declaring non-type template parameters with auto
#define	cnLibrary_CPPFEATURE_TEMPLATE_NONTYPE_AUTO	__cpp_nontype_template_parameter_auto

//- C++20 -------------------------------------------------------------------

// (C++20)201907L __cpp_deduction_guides			CTAD for aggregates and aliases
// (C++20)201907L __cpp_constexpr_dynamic_alloc		Operations for dynamic storage duration in constexpr functions
// (C++20)201902L __cpp_aggregate_paren_init		Aggregate initialization in the form of direct initialization
// (C++20)201806L __cpp_conditional_explicit		explicit(bool)
// (C++20)201711L __cpp_constexpr_in_decltype		Generation of function and variable definitions when needed for constant evaluation
// (C++20)201707L __cpp_designated_initializers		Designated initializer
// (C++20)201707L __cpp_fold_expressions			Explicit template parameter list for generic lambdas
// (C++20)201806L __cpp_impl_destroying_delete		Destroying operator delete
// (C++20)201907L __cpp_modules						Modules
// (C++20)201907L __cpp_using_enum					using enum
// (C++20)201911L __cpp_nontype_template_parameter_auto		Class types and floating-point types in non-type template parameters
// (C++20)201803L __has_cpp_attribute(likely)				[[likely]]
// (C++20)201803L __has_cpp_attribute(no_unique_address)	[[no_unique_address]]
// (C++20)201907L __has_cpp_attribute(nodiscard)			[[nodiscard]]
// (C++20)201803L __has_cpp_attribute(unlikely)				[[unlikely]]


// (C++20)201811L __cpp_char8_t						char8_t
// (C++23)202207L __cpp_char8_t						compatibility and portability fix (allow initialization of (unsigned) char arrays from UTF-8 string literals)
#define	cnLibrary_CPPFEATURE_UCHAR8			__cpp_char8_t

// (C++20)201902L __cpp_impl_coroutine				Coroutines
#define	cnLibrary_CPPFEATURE_COROUTINE		__cpp_impl_coroutine

// (C++20)201811L __cpp_consteval					Immediate functions
#define	cnLibrary_CPPFEATURE_CONSTEVAL		__cpp_consteval

// (C++20)201907L __cpp_constinit					constinit
#define	cnLibrary_CPPFEATURE_CONSTINIT		__cpp_constinit

// (C++20)201907L __cpp_concepts					Concepts
#define	cnLibrary_CPPFEATURE_CONCEPTS		__cpp_concepts


// (C++20)201907L __cpp_impl_three_way_comparison	Three-way comparison
#define	cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION	__cpp_impl_three_way_comparison

//- C++23 -------------------------------------------------------------------
// 
// (C++23)202110L __cpp_explicit_this_parameter		Explicit object parameter		
// (C++23)202106L __cpp_if_consteval				consteval if
// (C++23)202110L __cpp_multidimensional_subscript	Multidimensional subscript operator
// (C++23)202211L __cpp_multidimensional_subscript	static operator[]
// (C++23)202011L __cpp_size_t_suffix				Literal suffixes for size_t and its signed version
// (C++23)202207L __cpp_implicit_move				Simpler implicit move
// (C++23)202207L __cpp_named_character_escapes		Named universal character escapes
// (C++23)202207L __cpp_static_call_operator		static operator()

//---------------------------------------------------------------------------
#endif	/* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif





