// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "mmsgtestpropertywatcher.h"
#include "cmsgtestpropertywatcher.h"

static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);

EXPORT_C CMsgTestPropertyWatcher* CMsgTestPropertyWatcher::NewL(TUid aCategory, TUint aKey, MMsgTestPropertyWatcher& aCallback)
    {
    CMsgTestPropertyWatcher* self=new(ELeave) CMsgTestPropertyWatcher();
    CleanupStack::PushL(self);
    self->ConstructL(aCategory, aKey, aCallback);
    CleanupStack::Pop(self);
    return self;
    }

CMsgTestPropertyWatcher::CMsgTestPropertyWatcher()
    :CActive(EPriorityHigh)
    {}

void CMsgTestPropertyWatcher::ConstructL(TUid aCategory, TUint aKey, MMsgTestPropertyWatcher& aCallback)
    {
    iCategory = aCategory;
    iKey = aKey;
    iCallback = &aCallback;
   
    User::LeaveIfError(iProperty.Attach(aCategory,aKey)); 
    CActiveScheduler::Add(this);

    // initial subscription
    iProperty.Subscribe(iStatus);
    SetActive();
    }

EXPORT_C CMsgTestPropertyWatcher::~CMsgTestPropertyWatcher()
    {
    Cancel();
    iProperty.Close();
    }

void CMsgTestPropertyWatcher::DoCancel()
    {
    iProperty.Cancel();
    }

void CMsgTestPropertyWatcher::RunL()
    {
    // re-subscribe before processing new value to prevent missing updates
    iProperty.Subscribe(iStatus);
    SetActive();

    iCallback->HandleEventL(iProperty, iKey);
    }
    
EXPORT_C RProperty& CMsgTestPropertyWatcher::Property()
	{
	return iProperty;
	}
