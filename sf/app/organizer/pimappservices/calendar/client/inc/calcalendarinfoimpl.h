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

#ifndef __CALCALENDARINFOIMPL_H__
#define __CALCALENDARINFOIMPL_H__

#include <e32base.h>
#include <e32hashtab.h> 

#include "agmcalendarinfo.h"

class CCalSessionImpl;

NONSHARABLE_CLASS(CCalCalendarInfoImpl) : public CAgnCalendarInfo
    {
public:
    static CCalCalendarInfoImpl* NewL();
    static CCalCalendarInfoImpl* NewL(CCalSessionImpl& aSessionImpl);
    
    ~CCalCalendarInfoImpl();
    
    const TDesC8& PropertyValueL(const TDesC8& aKey) const;
    
private:
    CCalCalendarInfoImpl();
    CCalCalendarInfoImpl(CCalSessionImpl& aSessionImpl);
    
    void ConstructL();
    
private:
    CCalSessionImpl* iSessionImpl;
    };

    
#endif // __AGMCALENDARINFO_H__
