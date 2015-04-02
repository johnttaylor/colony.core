#ifndef Cpl_Container_RingBuffer_h_
#define Cpl_Container_RingBuffer_h_
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

#include <stdint.h>


/// 
namespace  Cpl { namespace Container { 


/** This template class implements a Ring Buffer.  The size of the
    ring buffer is only limited by available RAM and the data type
    of 'CNTTYPE'.  The default for CNTTYPE is 16bit, i.e. max elements
    is 64K.

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
        N:=         Maximum number of data elements that can be stored in the Ring Buffer
        CNTTYPE:=   Type of the variable that tracks the current element count (it also
                    limits the max number of data elements in the Ring Buffer).
 */
template <class ITEM, int N, class CNTTYPE=uint16_t>
class RingBuffer
{
private:
    /// Points to the first item in the buffer.
    ITEM*    m_headPtr;

    /// Points to the last item in the buffer.
    ITEM*    m_tailPtr;

    /// Current number of items in the buffer
    CNTTYPE  m_count;

    /// Storage for the Elements
    ITEM     m_elements[N];


public:
    /** Public constructor initializes head and tail pointers.
     */
    RingBuffer() throw();

	/** This is a special constructor for when the Ring Buffer is 
		statically declared (i.e. it is initialized as part of
		C++ startup BEFORE main() is executed.  C/C++ guarantees
		that all statically declared data will be initialized
		to zero by default (see r.8.4 in C++ Programming Language, 
		Second Edition).  Since the head & tail pointers are
		initialized to zero - then the C/C++ default is OK.  Why
		do I care about all this?  Because if you attempt to build
		static list(s), then the order of when the list is 
		constructed vs. when the static elements are added to the 
		list is a problem!  To avoid this problem, a alternate 
		constructor that does NOT initialize the head & tail pointers
		is provided.  It assumes that the pointers are already set 
		zero because the list is "static". Whew!
	 */
	 RingBuffer(const char* ignoreThisParameter_usedToCreateAUniqueConstructor) throw();


public:
    /** Removes the first item in the Buffer. The contents of the
        removed item will be copied into the 'dst' argument. The method
        return true if the operation was succesfull; else false is
        returned, i.e. the Ring buffer is/was empty.
     */
    bool remove( ITEM& dst ) throw();


    /** The contents of 'item' will be copied into the Ring Buffer as the
        'last' item in the  buffer. Return true if the operation was 
        succesfull; else false is returned, i.e. the Buffer was full prior to 
        the attempted add().
     */
    bool add( ITEM& item) throw();
	

    /** Returns a pointer to the first item in the Buffer.
        The returned item remains in the buffer.  Returns 0
        if the Buffer is empty.
     */
    ITEM* peekHead( void ) const throw();


    /** Returns a pointer to the last item in the Buffer.
        The returned item remains in the Buffer.  Returns 0
        if the Buffer is empty.
     */
    ITEM* peekTail(void) const throw();



public:
    /** This method returns true if the Ring Buffer is empty
     */
    bool isEmpty( void ) const throw();
    
    /** This method returns true if the Ring Buffer is full
     */
    bool isFull( void ) const throw();


    /** This method returns the current number of items in
        the Ring Buffer
     */
    CNTTYPE getNumItems( void ) const throw();
    

    /** This method returns the maximum number of items that
        can be stored in the Ring buffer.
     */
    CNTTYPE getMaxItems( void ) const throw();
     	


public:
	/** Empties the Ring Buffer.  All references to the item(s) in the
		buffer are lost.
	 */
	void clearTheBuffer() throw();



private: 
    /// Prevent access to the copy constructor -->Containers can not be copied!
    RingBuffer( const RingBuffer& m );

    /// Prevent access to the assignment operator -->Containers can not be copied!
    const RingBuffer& operator=( const RingBuffer& m );

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////


template <class ITEM, int N, class CNTTYPE>
RingBuffer<ITEM,N,CNTTYPE>::RingBuffer(void) throw()
:m_headPtr(0),m_tailPtr(0),m_count(0)
    {
    }

template <class ITEM, int N, class CNTTYPE>
RingBuffer<ITEM,N,CNTTYPE>::RingBuffer(const char* notUsed) throw()
    {
    }


template <class ITEM, int N, class CNTTYPE>
inline void RingBuffer<ITEM,N,CNTTYPE>::clearTheBuffer() throw()
	{
    m_count = 0;
	}


template <class ITEM, int N, class CNTTYPE>
inline bool RingBuffer<ITEM,N,CNTTYPE>::add( ITEM& item) throw()
    {
    if ( isFull() )
        {
        return false;
        }

    if ( isEmpty() )
        {
        m_headPtr = m_tailPtr = m_elements;
        }
    else
        {
        if ( ++m_tailPtr >= m_elements+N )
            {
            m_tailPtr = m_elements;
            }
        }

    *m_tailPtr = item;
    m_count++;
    return true;
    }

template <class ITEM, int N, class CNTTYPE>
inline bool RingBuffer<ITEM,N,CNTTYPE>::remove( ITEM& dst ) throw()
    {
    if ( isEmpty() )
        {
        return false;
        }

    m_count--;
    dst = *m_headPtr;
    if ( ++m_headPtr >= m_elements+N )
        {
        m_headPtr = m_elements;
        }
    return true;
	}
	

template <class ITEM, int N, class CNTTYPE>
inline ITEM* RingBuffer<ITEM,N,CNTTYPE>::peekHead( void ) const throw()
    {
    if ( isEmpty() )
        {
        return 0;
        }
    
    return m_headPtr;
    }

template <class ITEM, int N, class CNTTYPE>
inline ITEM* RingBuffer<ITEM,N,CNTTYPE>::peekTail( void ) const throw()
    {
    if ( isEmpty() )
        {
        return 0;
        }
    
    return m_tailPtr;
    }


template <class ITEM, int N, class CNTTYPE>                                                                                            
inline bool RingBuffer<ITEM,N,CNTTYPE>::isEmpty( void ) const throw()
    {
    return m_count == 0;
    }

template <class ITEM, int N, class CNTTYPE>                                                                                            
inline bool RingBuffer<ITEM,N,CNTTYPE>::isFull( void ) const throw()
    {
    return m_count == N;
    }

template <class ITEM, int N, class CNTTYPE>
inline CNTTYPE RingBuffer<ITEM,N,CNTTYPE>::getNumItems( void ) const throw()
    {
	return m_count;
	}
	
template <class ITEM, int N, class CNTTYPE>
inline CNTTYPE RingBuffer<ITEM,N,CNTTYPE>::getMaxItems( void ) const throw()
    {
	return N;
	}

	

};      // end namespaces
};
#endif  // end header latch
