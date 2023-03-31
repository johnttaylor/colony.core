/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
#include <stdlib.h>
#include <ios>

///// Provide a non-exception-throwing new/delete so exception code does NOT get
///// linked in (i.e. -fno-exception is NOT enough to get rid of exception code
///// from libstdc++.a)
//
//
//void *operator new( size_t size ) throw() { return malloc(size); }
//void *operator new[]( size_t size ) throw() { return malloc(size); }
//
//void operator delete( void* p ) throw() { free(p); }
//void operator delete[]( void* p ) throw() { free(p); }
//
//
///* The function __cxa_atexit() handles the static destructors. In a bare-metal 
//   system this function can be empty because application has no operating system 
//   to return to, and consequently the static destructors are never called.
//*/
//extern "C" int __cxa_atexit( void ) { return 0; }
//
////extern "C" int __cxa_atexit( void* object,
////                             void (*destructor)(void*),
////                             void* dso_handle
////                           )
////    {
////    return 0;
////    }
//
//extern "C" void __cxa_pure_virtual() { while (1); }

/*
** Method stubs to satisfy the linker -->NOTE: C++ Streams are NOT supported by this BSP
*/
std::ios_base::Init::Init()
{
}

std::ios_base::Init::~Init()
{
}
