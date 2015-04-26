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

#include "TextBlock.h"
#include "Cpl/Text/strip.h"
#include <string.h>

//
using namespace Cpl::Text;
using namespace Cpl::Text::Tokenizer;


static void removeCharacter_( char* charToRemovePtr ) throw();


///////////////////////////////
TextBlock::TextBlock( char* string, char delimiter, char terminator, char quote, char escape )
:m_base((char*) stripSpace(string))
,m_ptr(m_base)
,m_validTokens(true)
,m_terminatorFound(false)
,m_count(0)
    {
    // Flag an error if 'string' is null
    if ( !string )
        {
        m_validTokens = false;
        return;
        }

    // Do nothing (aka zero parameter count) if the string is empty AFTER removing whitespace
    if ( *m_ptr == '\0' )
        {
        return;
        }

    // Parse the string
    while( *m_ptr != '\0' )
        {
        // Check for teminator character
        if ( *m_ptr == terminator )
            {
            // Finalize the found token and STOP parsing
            m_terminatorFound = true;
            *m_ptr++          = '\0';
            m_count++;
            break;
            }

        // Check for delimiter
        if ( *m_ptr == delimiter )
            {
            // Finalize the found token
            *m_ptr++ = '\0';
            m_count++;

            // SPECIAL CASE when the delimiter is a space -->need to strip any leading spaces or 
            // each 'extra' space would be an empty parameter field
            if ( delimiter == ' ' )
                {
                m_ptr = (char*) stripSpace(m_ptr);
                }
            }

            
        // Start of Text String
        else if ( *m_ptr == quote )
            {
            // Remove the starting quote character
            removeCharacter_( m_ptr );
            m_validTokens = false;
            bool escaping = false;

            // consume the text string
            while( m_ptr != '\0' )
                {
                // process the esacped character
                if ( escaping )
                    {
                    escaping = false;
                    m_ptr++;
                    }
                  
                // Exit the text string
                else if ( *m_ptr == quote )
                    {
                    // Remove the ending quote character
                    removeCharacter_( m_ptr );
                    m_validTokens = true;
                    break;
                    }

                // trap escape sequence
                else if ( *m_ptr == escape )
                    {
                    escaping = true;
                    removeCharacter_( m_ptr );
                    }

                // Normal character
                else
                    {
                    m_ptr++;
                    }
                }
            }

        // Normal parameter character
        else
            {
            // nothing todo -->so move on to the next character
            m_ptr++;
            }
        } 
    }


///////////////////////////////
bool TextBlock::getParameter( String& dst, unsigned index ) const throw()
    {
    // Trap out-of-bounds index
    if ( !m_validTokens || index >= m_count )
        {
        return false;
        }

	const char* token = m_base;
    unsigned    i;
	for(i=0; i<index; i++)
		{
		while ( *token != '\0' )
			{
			token++;
			}
		token++;
		}
    
    dst = token;
    dst.removeLeadingSpaces();
    dst.removeTrailingSpaces();
	return true;        
    }


///////////////////////////////
void removeCharacter_( char* charToRemovePtr ) throw()
    {
    char*  nextChar = charToRemovePtr + 1;
    size_t len      = strlen( nextChar ) + 1;

	// the move will include the null terminator
    memmove( charToRemovePtr, nextChar, len );
    }



