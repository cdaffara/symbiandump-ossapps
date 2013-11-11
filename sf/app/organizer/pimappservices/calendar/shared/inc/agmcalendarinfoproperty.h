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

#ifndef __AGMCALENDARINFOPROPERTY_H__
#define __AGMCALENDARINFOPROPERTY_H__

#include <e32base.h>

#include "agmcalendarinfo.h"

class CAgnCalendarInfoProperty : public CBase
    {
public:
    enum TState
        {
        ENone,
        EModified,
        EDeleted
        };
    
public:
    static CAgnCalendarInfoProperty* NewLC(const TDesC8& aKey);
    ~CAgnCalendarInfoProperty();
    
    const TDesC8& Key() const;
    IMPORT_C const TDesC8& Value() const;
    
    IMPORT_C void SetValue(HBufC8* aValue);
    
    IMPORT_C void SetState(TState aState);
    IMPORT_C TState State() const;
    
    void SetStreamId(TStreamId aStreamId);
    IMPORT_C TStreamId StreamId() const;
    
    static TInt Compare(const CAgnCalendarInfoProperty& aLeft, const CAgnCalendarInfoProperty& aRight);
    IMPORT_C static TInt Compare(const TDesC8* aKey, const CAgnCalendarInfoProperty& aProperty);
    
private:
    CAgnCalendarInfoProperty();
    void ConstructL(const TDesC8& aKey);
    
private:
    HBufC8* iKey;
    HBufC8* iValue;
    TState iState;
    TStreamId iStreamId;
    };

#endif

