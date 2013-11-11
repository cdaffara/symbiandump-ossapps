// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __VERSITTLS_H__
#define __VERSITTLS_H__

// System includes
#include <e32base.h>
#include <charconv.h>

#include <vutil.h>

class CVersitTimer;
class CVersitAdditionalStorage;

class CVersitTlsData : public CBase
/** Versit thread local storage.

This class provides a performance improvement by allowing a CVersitUnicodeUtils 
instance to be shared between parsers operating in the same thread, so that 
a new instance does not have to be created for each parser. A pointer to the 
unicode utilities object is held in thread local storage: a single word (32bits) 
of data. Each unicode utilities object is managed by an instance of this class.

Every time a parser is created, CVersitParser::ConstructL() calls the 
CVersitTlsData constructor, and when the parser is destroyed the CVersitTlsData 
destructor is called. If a CVersitTlsData object exists, the constructor 
returns a pointer to it, otherwise a new one is constructed. The CVersitTlsData 
object is only destroyed when no more parsers refer to it: a count is kept, 
which is incremented every time the constructor is called and decremented each 
time the destructor is called, and the object is only destroyed when the count 
reaches zero.

This class provides an additional major performance improvement 
if you are sequentially constructing and destructing multiple parsers. 
By default, when the count of parsers reaches zero, the thread local 
storage object is destroyed (even if the thread has not finished). However, 
by using the technique described below, the thread local storage object, and therefore 
the unicode utilities object, can be made to persist, significantly reducing 
the overhead of sequentially constructing and destructing parsers.

The constructor needs to be called an extra time before creating any parsers, 
and the destructor needs to be called an extra time once the parsers have 
been destroyed. This has the effect of adding one to the reference count so 
that during all the parser construction and deletion the count never hits 
zero, which would trigger the TLS object's destruction.

This can be implemented as follows:

1. Create a thread local storage data class instance as follows: 
@code
CVersitTlsData* versitTLS = CVersitTlsData::VersitTlsDataL(); 
@endcode

2. Create and delete the parsers. 

3. Delete the Thread Local Storage Data class instance: 
@code
delete versitTLS; 
@endcode 
@publishedAll
@released
*/
	{
	friend class CVersitTimer;

public:
	IMPORT_C static CVersitTlsData& VersitTlsDataL();
	IMPORT_C static void CloseVersitTlsData();
	IMPORT_C void VersitTlsDataClose();

public:
	inline CVersitUnicodeUtils& UnicodeUtils() 
	/** Returns a pointer to the current Unicode utilities object.
	
	@return A pointer to the current Unicode utilities object. */
		{ return *iUnicodeUtils; }

    inline CVersitAdditionalStorage& AdditionalStorage()
	/** Returns a pointer to the additional property storage object.
	
	@return A pointer to the additional property storage. */
        {
        return *iAdditionalStorage;
        }

private:
	static CVersitTlsData* NewL();
	void ConstructL();
	~CVersitTlsData();

private:
	TInt iRefCount;
	CVersitUnicodeUtils* iUnicodeUtils;
    CVersitAdditionalStorage* iAdditionalStorage;
	};

#endif
