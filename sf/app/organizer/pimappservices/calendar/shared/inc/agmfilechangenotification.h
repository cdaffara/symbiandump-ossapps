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

#ifndef __AGMFILECHANGENOTIFICATION_H__
#define __AGMFILECHANGENOTIFICATION_H__  

#include <e32base.h>
#include <gdi.h>

#include "calfilechangenotification.h"

class RReadStream;
class RWriteStream;

/** The calendar info class where metadata of a calendar file is set and retrieved.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CAgnFileChangeInfo) : public CBase
    {
public:
    IMPORT_C static CAgnFileChangeInfo* NewL(HBufC* aFileName, MCalFileChangeObserver::TChangeType aChangeType);
    IMPORT_C static CAgnFileChangeInfo* NewL(RReadStream& aReadStream);
    IMPORT_C ~CAgnFileChangeInfo();
     
    /** Get the file name of the file that has changed */
    IMPORT_C const TDesC& FileNameL() const;
     
    /** Get the type of change that happened to the file */
    IMPORT_C MCalFileChangeObserver::TChangeType ChangeType() const;
    IMPORT_C void InternalizeL(RReadStream& aReadStream);
    IMPORT_C void ExternalizeL(RWriteStream& aWriteStream) const;
     
 private:
    CAgnFileChangeInfo();
    CAgnFileChangeInfo(HBufC* aFileName, MCalFileChangeObserver::TChangeType aChangeType);

 private:
    HBufC* iCalendarFileName;
    MCalFileChangeObserver::TChangeType iChangeType;
    };

#endif
