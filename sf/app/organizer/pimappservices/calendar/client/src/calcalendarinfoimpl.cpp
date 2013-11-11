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

#include "calcommonimpl.h"
#include "calcalendarinfoimpl.h"
#include "agmcalendarinfo.h"
#include "agmcalendarinfoproperty.h"
#include "calsessionimpl.h"
#include "calclient.h"

CCalCalendarInfoImpl* CCalCalendarInfoImpl::NewL()
    {
    CCalCalendarInfoImpl* self = new(ELeave) CCalCalendarInfoImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CCalCalendarInfoImpl* CCalCalendarInfoImpl::NewL(CCalSessionImpl& aSessionImpl)
    {
    CCalCalendarInfoImpl* self = new(ELeave) CCalCalendarInfoImpl(aSessionImpl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CCalCalendarInfoImpl::CCalCalendarInfoImpl()
    {
    }

CCalCalendarInfoImpl::CCalCalendarInfoImpl(CCalSessionImpl& aSessionImpl)
    :iSessionImpl(&aSessionImpl)
    {
    iSessionImpl->IncrementReferenceCount();
    }

void CCalCalendarInfoImpl::ConstructL()
    {
    CAgnCalendarInfo::ConstructL();
    }

CCalCalendarInfoImpl::~CCalCalendarInfoImpl()
    {
    if (iSessionImpl)
        {
        iSessionImpl->DecrementReferenceCount();
        }
    }

const TDesC8& CCalCalendarInfoImpl::PropertyValueL(const TDesC8& aKey) const
    {
    TInt position = iProperties.FindInOrder(aKey, CAgnCalendarInfoProperty::Compare);
    
    if (position != KErrNotFound)
        {
        if (iProperties[position]->Value().Compare(KNullDesC8) == 0)
            {
            __ASSERT_DEBUG(iFileName && iProperties[position]->StreamId() != KNullStreamId, CalUtils::Panic(ECalendarInfoNullStreamId));
            // Get the property
            HBufC8* value = iSessionImpl->Server().GetPropertyValueL(iFileName->Des(), iProperties[position]->StreamId());
            iProperties[position]->SetValue(value);
            }
        else if (iProperties[position]->State() == CAgnCalendarInfoProperty::EDeleted)
            {
            // The property is here, but has been flagged as deleted
            User::Leave(KErrNotFound);
            }
        
        return iProperties[position]->Value();
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    
    // We should never get here. This is just to stop the compiler complaing
    return KNullDesC8;
    
    }
	
// End of File
