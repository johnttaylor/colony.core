#ifndef Rte_Element_DataType_h_
#define Rte_Element_DataType_h_
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


///
namespace Rte { namespace Element {

/** This class defines the data type constants for the different
    element types.  The domain application can add its own data type
    by defining a series of integer constant that their starting value
    is START_APP_DEFINED. 
 */
class DataType
{
public:
    /// My Type Id
    const int m_dataTypeId;

public:
    /// Element Type ID
    static const int INT8       = 1;

    /// Element Type ID
    static const int INT16      = 2;

    /// Element Type ID
    static const int INT32      = 3;

    /// Element Type ID
    static const int INT64      = 4;

    /// Element Type ID
    static const int UINT8      = 5;

    /// Element Type ID
    static const int UINT16     = 6;

    /// Element Type ID
    static const int UINT32     = 7;

    /// Element Type ID
    static const int UINT64     = 8;

    /// Element Type ID
    static const int SIZE_T     = 9;

    /// Element Type ID
    static const int FLOAT      = 10;

    /// Element Type ID
    static const int DOUBLE     = 11;

    /// Element Type ID
    static const int VOIDPTR    = 12;

    /// Element Type ID
    static const int STRING     = 13;

    /// Element Type ID
    static const int BOOL       = 14;

    /// Element Type ID
    static const int TOKEN4     = 15;

    /// Element Type ID
    static const int MD5        = 16;

    /// Element Type ID
    static const int SEQNUM     = 17;

    /// Element Type ID
    static const int REFCOUNT   = 18;

    /// Element Type ID
    static const int TIME_T     = 19;

    /// Element Type ID
    static const int BITFLAGS   = 20;

    /// Element Type ID 
    static const int CORAL_ADDR = 21;


    /// Start of APPLICATION defined Element Type IDs
    static const int START_APP_DEFINED = 0x2000;            



public:
    /// Constructor
    DataType( int dataTypeId ):m_dataTypeId(dataTypeId)
        {
        }
    
public:
    /** Not equals comparsion.  Note: The Application should avoid 'equals' 
        comparision as this leads to testing for ALL types - which breaks
        down when new types are added.  However, checking for "this is not
        me" does scale when new types are added - hence the isNotEqual method
     */
    inline bool isNotEqual( DataType x ) const 
        { 
        return m_dataTypeId != x.m_dataTypeId; 
        }
};

};      // end namespaces
};
#endif  // end header latch
