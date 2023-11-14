#ifndef __cnLibrary_cnRTL_cnRTL_H__
#define __cnLibrary_cnRTL_cnRTL_H__

#include <cnRTL/cnRTLCore.h>

#define	cnLibrary_AVAILABLE_RTL

#ifdef cnLibrary_CPPEXCLUDE_NULLPTR
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 nullptr to compile")
#endif

#ifdef cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 virtual override to compile")
#endif

#ifdef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 class member deleted to compile")
#endif

#ifdef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 Default template arguments for function templates to compile")
#endif


#if cnLibrary_CPPFEATURE_DECLTYPE < 200707L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 decltype to compile")
#endif

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 variadic template to compile")
#endif

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 rvalue references to compile")
#endif

#if cnLibrary_CPPFEATURE_ALIAS_TEMPLATES < 200704L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 alias template to compile")
#endif

#if cnLibrary_CPPFEATURE_NSDMI < 200809L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 Non-static data member initializers to compile")
#endif // cnLibrary_CPPFEATURE_NSDMI < 200809L

#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 100
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 Inheriting constructors to compile")

#endif

#include <cnRTL/RuntimeFunction.h>
#include <cnRTL/String.h>
#include <cnRTL/ObjectLifeCycle.h>
#include <cnRTL/ObjectReference.h>
#include <cnRTL/InterfaceWrapper.h>
#include <cnRTL/InterfaceImplementation.h>

// Threading

#include <cnRTL/ThreadSynchronization.h>
#include <cnRTL/ThreadAsync.h>
#include <cnRTL/AsyncTask.h>

// 

#include <cnRTL/TextProcess.h>
#include <cnRTL/FunctionSet.h>

// Debug

#include <cnRTL/Logger.h>


// Stream

#include <cnRTL/Stream.h>
#include <cnRTL/StreamBuffer.h>
#include <cnRTL/StreamAdapter.h>
#include <cnRTL/StreamProtocol.h>

// Network

#include <cnRTL/NetConnection.h>

// Data Processing
#include <cnRTL/DataHash.h>


// DB
#include <cnRTL/DataModel.h>
#include <cnRTL/Database.h>


// UI

#include <cnRTL/UIEvent.h>
#include <cnRTL/UIGraph.h>

// console
#include <cnRTL/CommandLineConsole.h>

#include <cnRTL/XMLParse.h>


#endif

