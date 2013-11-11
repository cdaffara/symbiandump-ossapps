// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALCALENDARINFOITERATOR_H__
#define __CALCALENDARINFOITERATOR_H__

#include <e32base.h>

class CCalSession;
class CCalCalendarInfo;
class CCalCalendarIteratorImpl;

/** 
An iterator for iterating though all calendar file metadata.

The iterator returns CCalCalendarInfo objects for calendar files that 
have metadata set. The iterator is unordered.

The iterator implementation will cache filenames on the client side

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalCalendarIterator) : public CBase
    {
public:
    IMPORT_C static CCalCalendarIterator* NewL(CCalSession& aSession);
    IMPORT_C ~CCalCalendarIterator();
    
    IMPORT_C CCalCalendarInfo* FirstL(); 
    IMPORT_C CCalCalendarInfo* NextL();
    IMPORT_C CCalCalendarInfo* CurrentL();
    
    IMPORT_C TInt Count() const;

private:
    CCalCalendarIterator();
    void ConstructL(CCalSession& aSession);
    
private:
    CCalCalendarIteratorImpl* iImpl; 
    };

#endif
