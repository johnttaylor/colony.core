/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Basic.h"
#include "Cpl/Text/atob.h"
#include "Cpl/Math/real.h"
#include <stdint.h>




///
using namespace Rte::Element;


/////////////////////////////
Boolean::Boolean( bool    initialValue,
                      bool    inUse,
                      int8_t  validState
                    )
:Basic<bool,DataType::BOOL>( initialValue, inUse, validState )
    {
    }

Boolean::Boolean( int     specializedType,
                      bool    initialValue,
                      bool    inUse,
                      int8_t  validState
                    )
:Basic<bool,DataType::BOOL>( initialValue, inUse, validState, specializedType )
    {
    }



const char* Boolean::getTypeAsText(void) const
    {
    return "BOOL";
    }


const char* Boolean::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%s", m_data? "T": "F" ); 
        }

    return dstMemory; 
    }


const char* Boolean::setFromText( const char* srcText, const char* terminationChars )
    {
    if ( srcText )
        {
        if ( *srcText == 'T' || *srcText == 't' )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = true;
                }

            return srcText+1;
            }
        else if ( *srcText == 'F' || *srcText == 'f' )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = false;
                }

            return srcText+1;
            }
        }

    return 0;
    }





/////////////////////////////

Integer8::Integer8( int8_t initialValue, 
                        bool   inUse,
                        int8_t validState
                      )
:Basic<int8_t,DataType::INT8>(initialValue, inUse, validState )
    {
    }

const char* Integer8::getTypeAsText(void) const
    {
    return "INT8";
    }


const char* Integer8::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%d", m_data );
        }

    return dstMemory; 
    }


const char* Integer8::setFromText( const char* srcText, const char* terminationChars ) 
    {
    const char* endPtr = 0;
    int         temp;
    if ( Cpl::Text::a2i(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp <= INT8_MAX && temp >= INT8_MIN )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = (int8_t) temp;
                }

            return endPtr;
            }
        }

    return 0;
    }



/////////////////////////////
Uinteger8::Uinteger8( uint8_t initialValue,
                          bool    inUse,
                          int8_t  validState
                        )
:Basic<uint8_t,DataType::UINT8>( initialValue, inUse, validState )
    {
    }


const char* Uinteger8::getTypeAsText(void) const
    {
    return "UINT8";
    }


const char* Uinteger8::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%u", m_data );
        }

    return dstMemory; 
    }
    

const char* Uinteger8::setFromText( const char* srcText, const char* terminationChars )
    {
    const char* endPtr = 0;
    unsigned    temp;
    if ( Cpl::Text::a2ui(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp <= UINT8_MAX )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = (uint8_t) temp;
                }

            return endPtr;
            }
        }

    return 0;
    }



/////////////////////////////
Integer16::Integer16( int16_t initialValue,
                          bool    inUse,
                          int8_t  validState
                        )
:Basic<int16_t,DataType::INT16>( initialValue, inUse, validState )
    {
    }


const char* Integer16::getTypeAsText(void) const
    {
    return "INT16";
    }


const char* Integer16::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%d", m_data );
        }

    return dstMemory; 
    }


const char* Integer16::setFromText( const char* srcText, const char* terminationChars )
    {
    const char* endPtr = 0;
    int         temp;
    if ( Cpl::Text::a2i(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp <= INT16_MAX && temp >= INT16_MIN )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = (int16_t) temp;
                }

            return endPtr;
            }
        }

    return 0;
    }


/////////////////////////////
Uinteger16::Uinteger16( uint16_t initialValue,
                            bool     inUse,
                            int8_t   validState
                          )
:Basic<uint16_t,DataType::UINT16>( initialValue, inUse, validState )
    {
    }


const char* Uinteger16::getTypeAsText(void) const
    {
    return "UINT16";
    }


const char* Uinteger16::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%u", m_data );
        }

    return dstMemory; 
    }


const char* Uinteger16::setFromText( const char* srcText, const char* terminationChars )
    {
    const char* endPtr = 0;
    unsigned    temp;
    if ( Cpl::Text::a2ui(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp <= UINT16_MAX )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = (uint16_t) temp;
                }

            return endPtr;
            }
        }

    return 0;
    }


/////////////////////////////
Integer32::Integer32( int32_t initialValue,
                          bool    inUse,
                          int8_t  validState
                        )
:Basic<int32_t,DataType::INT32>( initialValue, inUse, validState )
    {
    }


const char* Integer32::getTypeAsText(void) const
    {
    return "INT32";
    }


const char* Integer32::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%ld", m_data );
        }

    return dstMemory; 
    }


const char* Integer32::setFromText( const char* srcText, const char* terminationChars )
    {
    const char* endPtr = 0;
    long        temp;
    if ( Cpl::Text::a2l(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp <= INT32_MAX && temp >= INT32_MIN )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = (int32_t) temp;
                }

            return endPtr;
            }
        }

    return 0;
    }


/////////////////////////////
Uinteger32::Uinteger32( uint32_t initialValue,
                            bool     inUse,
                            int8_t   validState
                          )
:Basic<uint32_t,DataType::UINT32>( initialValue, inUse, validState )
    {
    }


const char* Uinteger32::getTypeAsText(void) const
    {
    return "UINT32";
    }


const char* Uinteger32::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%lu", m_data );
        }

    return dstMemory; 
    }


const char* Uinteger32::setFromText( const char* srcText, const char* terminationChars )
    {
    const char* endPtr = 0;
    unsigned long temp;
    if ( Cpl::Text::a2ul(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp <= UINT32_MAX )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = (uint32_t) temp;
                }

            return endPtr;
            }
        }

    return 0;
    }


/////////////////////////////
Integer64::Integer64( int64_t initialValue,
                          bool    inUse,
                          int8_t  validState
                        )
:Basic<int64_t,DataType::INT64>( initialValue, inUse, validState )
    {
    }


const char* Integer64::getTypeAsText(void) const
    {
    return "INT64";
    }


const char* Integer64::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%lld", m_data );
        }

    return dstMemory; 
    }


const char* Integer64::setFromText( const char* srcText, const char* terminationChars )
    {
    const char* endPtr = 0;
    long long   temp;
    if ( Cpl::Text::a2ll(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( !isModelElement() || !isLocked() )
            {
            m_data = (int64_t) temp;
            }

        return endPtr;
        }

    return 0;
    }


/////////////////////////////
Uinteger64::Uinteger64( uint64_t initialValue,
                            bool     inUse,
                            int8_t   validState
                          )
:Basic<uint64_t,DataType::UINT64>( initialValue, inUse, validState )
    {
    }


const char* Uinteger64::getTypeAsText(void) const
    {
    return "UINT64";
    }


const char* Uinteger64::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%llu", m_data );
        }

    return dstMemory;
    }


const char* Uinteger64::setFromText( const char* srcText, const char* terminationChars )
    {
    const char*        endPtr = 0;
    unsigned long long temp;
    if ( Cpl::Text::a2ull(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( !isModelElement() || !isLocked() )
            {
            m_data = (uint64_t) temp;
            }

        return endPtr;
        }

    return 0;
    }



/////////////////////////////
SizeT::SizeT( size_t  initialValue,
                bool    inUse,
                int8_t  validState
              )
:Basic<size_t,DataType::SIZE_T>( initialValue, inUse, validState )
    {
    }


const char* SizeT::getTypeAsText(void) const
    {
    return "SIZE_T";
    }


const char* SizeT::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%p", (void*)m_data ); // NOTE: Microsoft VC12 does NOT support the "z" length modifier! 
        }

    return dstMemory;
    }


const char* SizeT::setFromText( const char* srcText, const char* terminationChars )
    {
    const char*        endPtr = 0;
    unsigned long long temp;
    if ( Cpl::Text::a2ull(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp <= ((size_t) -1 ) )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = (size_t)temp;
                }

            return endPtr;
            }
        }

    return 0;
    }


/////////////////////////////
TimeT::TimeT( time_t  initialValue,
                bool    inUse,
                int8_t  validState
              )
:Basic<time_t,DataType::TIME_T>( initialValue, inUse, validState )
    {
    }


const char* TimeT::getTypeAsText(void) const
    {
    return "TIME_T";
    }


const char* TimeT::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%lld", (long long)m_data ); 
        }

    return dstMemory;
    }


const char* TimeT::setFromText( const char* srcText, const char* terminationChars )
    {
    const char*  endPtr = 0;
    long long    temp;
    if ( Cpl::Text::a2ll(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp > 0 )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = (time_t)temp;
                }

            return endPtr;
            }
        }

    return 0;
    }



/////////////////////////////
Float::Float( float   initialValue,
                  bool    inUse,
                  int8_t  validState
                )
:Basic<float,DataType::FLOAT>( initialValue, inUse, validState )
    {
    }


bool Float::isDifferentFrom( const Api& other ) const
    {
    assertTypeMatches( other );
    return Cpl::Math::areFloatsEqual( m_data, *((float*)(other.dataPointer())) ) == false;
    }


const char* Float::getTypeAsText(void) const
    {
    return "FLOAT";
    }


const char* Float::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%.9G", m_data );
        }

    return dstMemory;
    }


const char* Float::setFromText( const char* srcText, const char* terminationChars )
    {
    const char* endPtr = 0;
    double      temp;
    if ( Cpl::Text::a2d(temp, srcText, terminationChars, &endPtr) )
        {
        if ( !isModelElement() || !isLocked() )
            {
            m_data = (float)temp;
            }

        return endPtr;
        }

    return 0;
    }


/////////////////////////////
Double::Double( double   initialValue,
                    bool    inUse,
                    int8_t  validState
                  )
:Basic<double,DataType::DOUBLE>( initialValue, inUse, validState )
    {
    }


bool Double::isDifferentFrom( const Api& other ) const
    {
    assertTypeMatches( other );
    return Cpl::Math::areDoublesEqual( m_data, *((double*)(other.dataPointer())) ) == false;
    }


const char* Double::getTypeAsText(void) const
    {
    return "DOUBLE";
    }


const char* Double::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%.17G", m_data );
        }

    return dstMemory;
    }


const char* Double::setFromText( const char* srcText, const char* terminationChars )
    {
    const char* endPtr = 0;
    double      temp;
    if ( Cpl::Text::a2d(temp, srcText, terminationChars, &endPtr) )
        {
        if ( !isModelElement() || !isLocked() )
            {
            m_data = temp;
            }

        return endPtr;
        }

    return 0;
    }


/////////////////////////////
VoidPtr::VoidPtr( void*   initialValue,
                      bool    inUse,
                      int8_t  validState
                    )
:Basic<void*,DataType::VOIDPTR>( initialValue, inUse, validState )
    {
    }


const char* VoidPtr::getTypeAsText(void) const
    {
    return "VOIDPTR";
    }


const char* VoidPtr::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%0*p", sizeof(void*)*2, m_data );
        }

    return dstMemory;
    }


const char* VoidPtr::setFromText( const char* srcText, const char* terminationChars )
    {
    const char*        endPtr = 0;
    unsigned long long temp;
    if ( Cpl::Text::a2ull(temp, srcText, 0, terminationChars, &endPtr ) )
        {
        if ( temp <= ((size_t) -1 ) )
            {
            if ( !isModelElement() || !isLocked() )
                {
                m_data = (void*)temp;
                }

            return endPtr;
            }
        }

    return 0;
    }
