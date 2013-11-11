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

#include "agmcalendarinfoproperty.h"

CAgnCalendarInfoProperty* CAgnCalendarInfoProperty::NewLC(const TDesC8& aKey)
    {
    CAgnCalendarInfoProperty* self = new(ELeave) CAgnCalendarInfoProperty;
    CleanupStack::PushL(self);
    self->ConstructL(aKey);
    return self;
    }

CAgnCalendarInfoProperty::~CAgnCalendarInfoProperty()
    {
    delete iKey;
    delete iValue;
    }
    
CAgnCalendarInfoProperty::CAgnCalendarInfoProperty()
    {
    }

void CAgnCalendarInfoProperty::ConstructL(const TDesC8& aKey)
    {
    iKey = aKey.AllocL();
    }

TInt CAgnCalendarInfoProperty::Compare(const CAgnCalendarInfoProperty& aLeft, const CAgnCalendarInfoProperty& aRight)
    {
    return Compare(aLeft.iKey, aRight);
    }

EXPORT_C TInt CAgnCalendarInfoProperty::Compare(const TDesC8* aKey, const CAgnCalendarInfoProperty& aProperty)
    {
    return aKey->CompareF(*aProperty.iKey);
    }

const TDesC8& CAgnCalendarInfoProperty::Key() const
    {
    return *iKey;
    }

EXPORT_C const TDesC8& CAgnCalendarInfoProperty::Value() const
    {
    if (iValue)
        {
        return *iValue;
        }
    
    return KNullDesC8;
    }

EXPORT_C void CAgnCalendarInfoProperty::SetValue(HBufC8* aValue)
    {
    delete iValue;
    iValue = aValue;
    iState = EModified;
    }

EXPORT_C void CAgnCalendarInfoProperty::SetState(TState aState)
    {
    iState = aState;
    }

EXPORT_C CAgnCalendarInfoProperty::TState CAgnCalendarInfoProperty::State() const
    {
    return iState;
    }
 
void CAgnCalendarInfoProperty::SetStreamId(TStreamId aStreamId)
    {
    iStreamId = aStreamId;
    }

EXPORT_C TStreamId CAgnCalendarInfoProperty::StreamId() const
    {
    return iStreamId;
    }
