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
#include "calfilechangenotification.h"
#include "agmfilechangenotification.h"

CCalFileChangeInfo* CCalFileChangeInfo::NewL(CAgnFileChangeInfo* aAgnFileChangeInfo)
    {
    CCalFileChangeInfo* self = new (ELeave) CCalFileChangeInfo(aAgnFileChangeInfo);
    return self;
    }

CCalFileChangeInfo::~CCalFileChangeInfo()
    {
    delete iImpl;
    }
    
    /** Get the file name of the file that has changed */
EXPORT_C const TDesC& CCalFileChangeInfo::FileNameL() const
    {
    return iImpl->FileNameL();
    }
    
    /** Get the type of change that happened to the file */
EXPORT_C MCalFileChangeObserver::TChangeType CCalFileChangeInfo::ChangeType() const
    {
    return iImpl->ChangeType();
    }
    

CCalFileChangeInfo::CCalFileChangeInfo(CAgnFileChangeInfo* aAgnFileChangeInfo)
    :iImpl(aAgnFileChangeInfo)
    {   
    }
