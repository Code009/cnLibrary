#ifdef cnLibrary_CPPEXCLUDE_NULLPTR
#define	nullptr	0
#endif // cnLibrary_CPPEXCLUDE_NULLPTR

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnLib_CONSTVAR	constexpr
#define	cnLib_CONSTEXPR_FUNC constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnLib_CONSTVAR	const
#define	cnLib_CONSTEXPR_FUNC
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L

#if	cnLibrary_CPPFEATURE_IF_CONSTEXPR >= 201606L
#define	cnLib_IFCONSTEXPR	constexpr
#else	// cnLibrary_CPPFEATURE_IF_CONSTEXPR < 201606L
#define	cnLib_IFCONSTEXPR
#endif	// cnLibrary_CPPFEATURE_IF_CONSTEXPR


#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
#define	cnLib_UREF				&&
#define	cnLib_UREFCAST(_type_)	static_cast<_type_&&>
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
#define	cnLib_UREF
#define	cnLib_UREFCAST(_type_)
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES

#if	cnLibrary_CPPFEATURE_STATIC_ASSERT >= 200410L
#define	cnLib_STATIC_ASSERT		static_assert
#else
#define	cnLib_STATIC_ASSERT(__exp__,__str__)
#endif
	
