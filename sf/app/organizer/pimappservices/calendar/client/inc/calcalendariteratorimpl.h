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

#ifndef __CALCALENDARINFOITERATORIMPL_H__
#define __CALCALENDARINFOITERATORIMPL_H__

#include <e32base.h>
#include <badesca.h>

#include "calcommon.h"

class CCalCalendarInfo;
class CCalSession;

NONSHARABLE_CLASS(CCalCalendarIteratorImpl) : public CBase
    {
public:
    static CCalCalendarIteratorImpl* NewL(CCalSession& aSession);
    ~CCalCalendarIteratorImpl();
    
    CCalCalendarInfo* FirstL(); 
    CCalCalendarInfo* NextL();
    CCalCalendarInfo* CurrentL();
    
    TInt Count() const;

private:
    CCalCalendarIteratorImpl(CCalSession& aSession);
    void ConstructL();
    
private:
    CCalSession& iSession;
    
    CDesCArray* iCalendarFiles;
    TInt iCurrentItem;
    };

#endif

