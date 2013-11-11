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

#include <s32strm.h>

#include "agmcalendarinfo.h"
#include "agmfilechangenotification.h"

/* Allocates and constructs a CAgnFileChangeInfo.
 * It takes the ownership of aFileName 
 */
EXPORT_C CAgnFileChangeInfo* CAgnFileChangeInfo::NewL(HBufC* aFileName, MCalFileChangeObserver::TChangeType aChangeType)
    {
    CAgnFileChangeInfo* self = new (ELeave) CAgnFileChangeInfo(aFileName, aChangeType);
    return self;    
    }

EXPORT_C CAgnFileChangeInfo* CAgnFileChangeInfo::NewL(RReadStream& aReadStream)
    {
    CAgnFileChangeInfo* self = new(ELeave) CAgnFileChangeInfo();
    CleanupStack::PushL(self);
    self->InternalizeL(aReadStream);
    CleanupStack::Pop(self);
    return self;   
    }

EXPORT_C CAgnFileChangeInfo::~CAgnFileChangeInfo()
    {
    delete iCalendarFileName;
    }
 
 /** Get the file name of the file that has changed */
EXPORT_C const TDesC& CAgnFileChangeInfo::FileNameL() const
    {
    if(!iCalendarFileName)
        {
        return KNullDesC;
        }
    return *iCalendarFileName;
    }
 
 /** Get the type of change that happened to the file */
EXPORT_C MCalFileChangeObserver::TChangeType CAgnFileChangeInfo::ChangeType() const
    {
    return iChangeType;
    }

EXPORT_C void CAgnFileChangeInfo::InternalizeL(RReadStream& aReadStream)
    {
    iChangeType = static_cast<MCalFileChangeObserver::TChangeType>(aReadStream.ReadInt32L());
    iCalendarFileName = HBufC::NewL(aReadStream, KMaxTInt);
    }

EXPORT_C void CAgnFileChangeInfo::ExternalizeL(RWriteStream& aWriteStream) const
    {
    aWriteStream.WriteInt32L(iChangeType);
    aWriteStream << *iCalendarFileName;
    }

CAgnFileChangeInfo::CAgnFileChangeInfo(HBufC* aFileName, MCalFileChangeObserver::TChangeType aChangeType)
    :iCalendarFileName(aFileName),
    iChangeType(aChangeType)
    {   
    }

CAgnFileChangeInfo::CAgnFileChangeInfo()
    {
    }

