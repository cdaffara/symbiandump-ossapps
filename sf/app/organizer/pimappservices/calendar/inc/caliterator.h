// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALITER_H__
#define __CALITER_H__


#include <e32base.h>

class CCalSession;
class CCalIteratorImpl;

/** 
An iterator for iterating though all the entries in the calendar store.

This may be used by a synchronisation application to iterate all entries in the file to find entries that
have been added\\deleted between two synchronisations. 

The application must not create the entry view or instance view while iterating through the entries. 

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalIter) : public CBase

    {
public:
	IMPORT_C static CCalIter* NewL(CCalSession& aSession);
	IMPORT_C ~CCalIter();

	IMPORT_C const TDesC8& FirstL();
	IMPORT_C const TDesC8& NextL();

private:
	CCalIter();
	void ConstructL(CCalSession& aSession);

private:
	CCalIteratorImpl* iCalIteratorImpl;
	};
	
#endif // __CALITER_H__
