#ifndef Cpl_Text_String_x_h_
#define Cpl_Text_String_x_h_
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
/** @file */

#include "Cpl/Text/String.h"


///
namespace Cpl { namespace Text {


/** This partially concrete class implements the portions of the String Api 
    that is independent of the internal storage mechnicisms.
 */

class String_: public String
{
protected:
    ///
    bool  m_truncated;
    ///
    char* m_strPtr;


protected:
    /// Constructor
    String_( char* internalStringPtr );


public:
    ///
    void clear();
    ///
    operator char () const;
    ///
    int  length() const;
    ///
    bool isEmpty() const;
    ///
    bool truncated() const;
    ///
    bool operator ==(const char* string) const;
    ///
    bool operator ==(const String& string) const;
    ///
    bool operator !=(const char* string) const;
    ///
    bool operator !=(const String& string) const;
    ///
    bool isEqualIgnoreCase(const char* string) const;
    ///
    int  compare( const char* string ) const;
    ///
    int  compareIgnoreCase( const char* string ) const;
    ///
    bool operator ==(char c) const;
    ///
    bool isEqualSubstring(int startpos, int endpos, const char* string) const;
    /// 
    bool isEqualSubstringIgnoreCase(int startpos, int endpos, const char* string) const;
    /// 
    bool startsWith( const char* string ) const;
    /// 
    bool startsWith( const char* string, int startOffset ) const;
    /// 
    bool endsWith( const char* string ) const;
    ///
    void format( const char* format, ... );
    ///
    void formatAppend( const char* format, ... );
    ///
    void vformat( const char* format, va_list ap );
    ///
    void vformatAppend( const char* format, va_list ap );
    ///
    int  indexOf( char c ) const;
    ///
    int  indexOf( char c, int startpos ) const;
    ///
    int  indexOf( const char* str ) const;
    ///
    int  indexOf( const char* str, int startpos ) const;
    ///
    int  lastIndexOf( char c ) const;
    ///
    int count( char c ) const;
    /// 
    const char* toUpper();
    /// 
    const char* toLower();
    ///
    void removeLeadingSpaces();
    /// 
    void removeTrailingSpaces(); 
    ///
    void removeLeadingChars(const char* charsSet);
    /// 
    void removeTrailingChars(const char* charsSet);
    ///
    void cut(int startpos, int endpos);
	///
	void trimRight(int n);
    ///
    void setChar( int atPosition, char newchar );
	///
	int  replace(char targetChar, char newChar);
    ///
    const char* getString() const;
    ///
    char* getBuffer( int& maxAllowedLength );

public: /// Cpl::Container::Key interface
    ///
    int compareKey( const Key& key ) const;
    ///
    const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const;



protected: // Helpers
    /// Sets the truncated flag if needed and insures string is null terminated
    void validateSizeAfterFormat(int availableLen, int formattedLen, int maxlen);

};

};      // end namespaces
};
#endif  // end header latch
