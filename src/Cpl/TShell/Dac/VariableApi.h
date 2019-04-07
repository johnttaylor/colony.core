#ifndef Cpl_TShell_Dac_VariableApi_h_
#define Cpl_TShell_Dac_VariableApi_h_
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
/** @file */

#include "colony_config.h"
#include "Cpl/Container/MapItem.h"

/** This symbol defines the minimum size of the string (not including the null 
    terminator) used to store a Variable's name.
 */
#ifndef OPTION_CPL_TSHELL_DAC_VARIABLEAPI_NAME_SIZE
#define OPTION_CPL_TSHELL_DAC_VARIABLEAPI_NAME_SIZE    16
#endif

/** This symbol defines the minimum size of the string (not including the null 
    terminator) used to store a Variable's value.
 */
#ifndef OPTION_CPL_TSHELL_DAC_VARIABLEAPI_VALUE_SIZE
#define OPTION_CPL_TSHELL_DAC_VARIABLEAPI_VALUE_SIZE   64
#endif



///
namespace Cpl { namespace TShell { namespace Dac {

/** This class defines the interface a Debug shell Variable.

    NOTES:

        o All variables values are stored as strings

        o Numeric operation are performed by converting the 'string' to 
          a binary data type (e.g. double).  Once the numeric operation has 
          completed, then the binary value is converted back to a string 
          representation. 
 */

class VariableApi: public Cpl::Container::MapItem
{
public:
    /** This method returns the variable's name
     */
    virtual const char* getName() const noexcept = 0;


    /** This method returns the current value of the variable
     */
    virtual const char* getValue() const noexcept = 0;

    /// Overload the Cast-to-read-only-character-string-pointer as an alternate to getValue()
    inline operator const char* () const { return getValue(); }

    /** This method set the value of the variable.  The method returns
        true if succesful; else false is returned. An example of 
        a failed set call is the length of the new value exceeds the
        internal storage of the Variable.
     */
    virtual bool setValue( const char* newValue ) noexcept = 0;

    /** This method converts the binary numeric value to a string and
        sets the value of the Variable to the converted value. The method 
        returns true if succesful; else false is returned. An example of 
        a failed set call is the length of the new value exceeds the
        internal storage of the Variable.
     */
    virtual bool setValue( double newValue ) noexcept = 0;

    /// Same as above
    virtual bool setValue( long newValue ) noexcept = 0;

    /// Same as above
    virtual bool setValue( unsigned long newValue ) noexcept = 0;

    /// Similiar to setValue() methods above, but sets the value as an double or long depending on if there is/is-not an fractional digits
    virtual bool setNumericValue( double newValue ) noexcept = 0;


public:
    /// Overload the Cast to Cpl::Container::Key as a short-cut to getKey()
    inline operator const Cpl::Container::Key& () const { return getKey(); }


public:
    /** Adds the specified amount to the variable.  If 'amount' is not
        a valid 'numeric' value OR the variable's value is a not valid
        'numeric' value then false is returned; else true is returned.
     */
    virtual bool add( const char* amount ) noexcept = 0;


public:
    /** Attempts to convert the Variable's value to a binary form.  The
        method returns true if the conversation was successful; else
        false is returned.
     */
    virtual bool getNumber( double& valueAsNumber ) const noexcept = 0;

    /// Same as above
    virtual bool getNumber( long& valueAsNumber ) const noexcept = 0;

    /// Same as above
    virtual bool getNumber( unsigned long& valueAsNumber ) const noexcept = 0;


public:
    /** This method compares the two specified values and returns -1, 0, 1
        if 'leftValue' is less than, equal, or greater than 'rightValue'
        respectively.  If either 'leftValue' or 'rightValue' are not valid
        numeric values then the comparision is done as a string compare,
        i.e. strcmp().  If both values represent numeric values then the 
        comparision is done mathematically.                                
     */
    static int compare( const char* leftValue, const char* rightValue );


public:
    /// Virtual destructor
    virtual ~VariableApi(){}

};

};      // end namespaces
};
};
#endif  // end header latch
