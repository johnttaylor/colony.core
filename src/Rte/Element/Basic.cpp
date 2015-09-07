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

#include "Basic.h"
#include "Cpl/Text/atob.h"


///
using namespace Rte::Element;


/////////////////////////////
Boolean_T::Boolean_T( bool    initialValue,
                      bool    inUse,
                      int8_t  validState
                    )
:Basic<bool,DataType::BOOL>( initialValue, inUse, validState )
    {
    }


const char* Boolean_T::getTypeAsText(void) const
    {
    return "BOOL";
    }


const char* Boolean_T::toString( Cpl::Text::String& dstMemory ) const
    {
    return m_data? "T": "F";
    }


bool Boolean_T::setFromText( const char* srcText )
    {
    if ( *srcText == 'T' )
        {
        m_data = true;
        return true;
        }
    else if ( *srcText == 'F' )
        {
        m_data = false;
        return true;
        }

    return false;
    }



/////////////////////////////

Integer8_T::Integer8_T( int8_t initialValue, 
                        bool   inUse,
                        int8_t validState
                      )
:Basic<int8_t,DataType::INT8>(initialValue, inUse, validState )
    {
    }

const char* Integer8_T::getTypeAsText(void) const
    {
    return "INT8";
    }


const char* Integer8_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory = m_data;
    return dstMemory;
    }


bool Integer8_T::setFromText( const char* srcText ) 
    {
    int temp;
    if ( Cpl::Text::a2i(temp, srcText, 0 ) )
        {
        if ( (temp ^ 0xff) == 0 )
            {
            m_data = temp;
            return true;
            }
        }

    return false;
    }



/////////////////////////////
Uinteger8_T::Uinteger8_T( uint8_t initialValue,
                          bool    inUse,
                          int8_t  validState
                        )
:Basic<uint8_t,DataType::UINT8>( initialValue, inUse, validState )
    {
    }


const char* Uinteger8_T::getTypeAsText(void) const
    {
    return "UINT8";
    }


const char* Uinteger8_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory = m_data;
    return dstMemory;
    }
    

bool Uinteger8_T::setFromText( const char* srcText )
    {
    unsigned temp;
    if ( Cpl::Text::a2ui(temp, srcText, 0 ) )
        {
        if ( temp <= 0xff )
            {
            m_data = temp;
            return true;
            }
        }

    return false;
    }



/////////////////////////////
Integer16_T::Integer16_T( int16_t initialValue,
                          bool    inUse,
                          int8_t  validState
                        )
:Basic<int16_t,DataType::INT16>( initialValue, inUse, validState )
    {
    }


const char* Integer16_T::getTypeAsText(void) const
    {
    return "INT16";
    }


const char* Integer16_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory = m_data;
    return dstMemory;
    }


bool Integer16_T::setFromText( const char* srcText )
    {
    int temp;
    if ( Cpl::Text::a2i(temp, srcText, 0 ) )
        {
        if ( (temp ^ 0xffff) == 0 )
            {
            m_data = temp;
            return true;
            }
        }

    return false;
    }


/////////////////////////////
Uinteger16_T::Uinteger16_T( uint16_t initialValue,
                            bool     inUse,
                            int8_t   validState
                          )
:Basic<uint16_t,DataType::UINT16>( initialValue, inUse, validState )
    {
    }


const char* Uinteger16_T::getTypeAsText(void) const
    {
    return "UINT16";
    }


const char* Uinteger16_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory = m_data;
    return dstMemory;
    }


bool Uinteger16_T::setFromText( const char* srcText )
    {
    unsigned temp;
    if ( Cpl::Text::a2ui(temp, srcText, 0 ) )
        {
        if ( temp <= 0xffff )
            {
            m_data = temp;
            return true;
            }
        }

    return false;
    }


/////////////////////////////
Integer32_T::Integer32_T( int32_t initialValue,
                          bool    inUse,
                          int8_t  validState
                        )
:Basic<int32_t,DataType::INT32>( initialValue, inUse, validState )
    {
    }


const char* Integer32_T::getTypeAsText(void) const
    {
    return "INT32";
    }


const char* Integer32_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory = m_data;
    return dstMemory;
    }


bool Integer32_T::setFromText( const char* srcText )
    {
    long temp;
    if ( Cpl::Text::a2l(temp, srcText, 0 ) )
        {
        if ( (temp ^ 0xffffffff) == 0 )
            {
            m_data = temp;
            return true;
            }
        }

    return false;
    }


/////////////////////////////
Uinteger32_T::Uinteger32_T( uint32_t initialValue,
                            bool     inUse,
                            int8_t   validState
                          )
:Basic<uint32_t,DataType::UINT32>( initialValue, inUse, validState )
    {
    }


const char* Uinteger32_T::getTypeAsText(void) const
    {
    return "UINT32";
    }


const char* Uinteger32_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory = m_data;
    return dstMemory;
    }


bool Uinteger32_T::setFromText( const char* srcText )
    {
    unsigned long temp;
    if ( Cpl::Text::a2ul(temp, srcText, 0 ) )
        {
        if ( temp <= 0xffffffff )
            {
            m_data = temp;
            return true;
            }
        }

    return false;
    }


/////////////////////////////
Integer64_T::Integer64_T( int64_t initialValue,
                          bool    inUse,
                          int8_t  validState
                        )
:Basic<int64_t,DataType::INT64>( initialValue, inUse, validState )
    {
    }


const char* Integer64_T::getTypeAsText(void) const
    {
    return "INT64";
    }


const char* Integer64_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory = m_data;
    return dstMemory;
    }


bool Integer64_T::setFromText( const char* srcText )
    {
    long long temp;
    if ( Cpl::Text::a2ll(temp, srcText, 0 ) )
        {
        m_data = temp;
        return true;
        }

    return false;
    }


/////////////////////////////
Uinteger64_T::Uinteger64_T( uint64_t initialValue,
                            bool     inUse,
                            int8_t   validState
                          )
:Basic<uint64_t,DataType::UINT64>( initialValue, inUse, validState )
    {
    }


const char* Uinteger64_T::getTypeAsText(void) const
    {
    return "UINT64";
    }


const char* Uinteger64_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory = m_data;
    return dstMemory;
    }


bool Uinteger64_T::setFromText( const char* srcText )
    {
    unsigned long long temp;
    if ( Cpl::Text::a2ull(temp, srcText, 0 ) )
        {
        m_data = temp;
        return true;
        }

    return false;
    }



/////////////////////////////
Size_T::Size_T( size_t  initialValue,
                bool    inUse,
                int8_t  validState
              )
:Basic<size_t,DataType::SIZE_T>( initialValue, inUse, validState )
    {
    }


const char* Size_T::getTypeAsText(void) const
    {
    return "SIZE_T";
    }


const char* Size_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory = m_data;
    return dstMemory;
    }


bool Size_T::setFromText( const char* srcText )
    {
    unsigned long long temp;
    if ( Cpl::Text::a2ull(temp, srcText, 0 ) )
        {
        if ( temp <= ((size_t) -1 ) )
            {
            m_data = (size_t)temp;
            return true;
            }
        }

    return false;
    }


/////////////////////////////
Float_T::Float_T( float   initialValue,
                  bool    inUse,
                  int8_t  validState
                )
:Basic<float,DataType::FLOAT>( initialValue, inUse, validState )
    {
    }


const char* Float_T::getTypeAsText(void) const
    {
    return "FLOAT";
    }


const char* Float_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory.format( "%G", m_data );
    return dstMemory;
    }


bool Float_T::setFromText( const char* srcText )
    {
    double temp;
    if ( Cpl::Text::a2d(temp, srcText) )
        {
        m_data = (float) temp;
        }

    return false;
    }


/////////////////////////////
Double_T::Double_T( double   initialValue,
                    bool    inUse,
                    int8_t  validState
                  )
:Basic<double,DataType::DOUBLE>( initialValue, inUse, validState )
    {
    }


const char* Double_T::getTypeAsText(void) const
    {
    return "DOUBLE";
    }


const char* Double_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory.format( "%G", m_data );
    return dstMemory;
    }


bool Double_T::setFromText( const char* srcText )
    {
    double temp;
    if ( Cpl::Text::a2d(temp, srcText) )
        {
        m_data = temp;
        }

    return false;
    }


/////////////////////////////
VoidPtr_T::VoidPtr_T( void*   initialValue,
                      bool    inUse,
                      int8_t  validState
                    )
:Basic<void*,DataType::VOIDPTR>( initialValue, inUse, validState )
    {
    }


const char* VoidPtr_T::getTypeAsText(void) const
    {
    return "VOIDPTR";
    }


const char* VoidPtr_T::toString( Cpl::Text::String& dstMemory ) const
    {
    dstMemory.format( "%0*X", sizeof(void*)*2, m_data );
    return dstMemory;
    }


bool VoidPtr_T::setFromText( const char* srcText )
    {
    unsigned long long temp;
    if ( Cpl::Text::a2ull(temp, srcText, 0 ) )
        {
        if ( temp <= ((size_t) -1 ) )
            {
            m_data = (void*) temp;
            return true;
            }
        }

    return false;
    }
