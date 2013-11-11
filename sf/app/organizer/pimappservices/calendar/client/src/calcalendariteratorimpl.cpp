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

#include "calsession.h"
#include "calsessionimpl.h"
#include "calcalendariteratorimpl.h"
#include "calclient.h"
#include "calcalendarinfo.h"
#include "agmcalendarinfo.h"

CCalCalendarIteratorImpl* CCalCalendarIteratorImpl::NewL(CCalSession& aSession)
    {
    CCalCalendarIteratorImpl* self = new(ELeave) CCalCalendarIteratorImpl(aSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CCalCalendarIteratorImpl::CCalCalendarIteratorImpl(CCalSession& aSession)
    :iSession(aSession)
    {
    iSession.Impl().IncrementReferenceCount();
    }

void CCalCalendarIteratorImpl::ConstructL()
    {    
    iCalendarFiles = iSession.Impl().Server().ListAgendaFilesL();
    }

CCalCalendarIteratorImpl::~CCalCalendarIteratorImpl()
    {
    delete iCalendarFiles;
    
    iSession.Impl().DecrementReferenceCount();
    }


CCalCalendarInfo* CCalCalendarIteratorImpl::FirstL()
    {
    CCalCalendarInfo* info(NULL);
    
    iCurrentItem = 0;
    
    if (iCalendarFiles->Count() > iCurrentItem)
        {
        CCalCalendarInfoImpl* calInfoImpl = iSession.Impl().Server().GetCalendarInfoLC((*iCalendarFiles)[iCurrentItem], iSession.Impl());
        info = CCalCalendarInfo::NewL(calInfoImpl);
        CleanupStack::Pop(calInfoImpl);
        }
    
    return info;
    }

CCalCalendarInfo* CCalCalendarIteratorImpl::NextL()
    {
    CCalCalendarInfo* info(NULL);
    
    if (iCalendarFiles->Count() > ++iCurrentItem)
        {
        CCalCalendarInfoImpl* calInfoImpl = iSession.Impl().Server().GetCalendarInfoLC((*iCalendarFiles)[iCurrentItem], iSession.Impl());
        info = CCalCalendarInfo::NewL(calInfoImpl);
        CleanupStack::Pop(calInfoImpl);
        }
    
    return info;
    }

CCalCalendarInfo* CCalCalendarIteratorImpl::CurrentL()
    {
    CCalCalendarInfo* info(NULL);
    
    if (iCalendarFiles->Count() > iCurrentItem)
        {
        CCalCalendarInfoImpl* calInfoImpl = iSession.Impl().Server().GetCalendarInfoLC((*iCalendarFiles)[iCurrentItem], iSession.Impl());
        info = CCalCalendarInfo::NewL(calInfoImpl);
        CleanupStack::Pop(calInfoImpl);
        }
    
    return info;
    }

TInt CCalCalendarIteratorImpl::Count() const
    {
    return iCalendarFiles->Count();
    }

