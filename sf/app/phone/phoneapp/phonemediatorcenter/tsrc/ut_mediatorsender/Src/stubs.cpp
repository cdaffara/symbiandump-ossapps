/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <eunitmacros.h>
#include <e32std.h>

#include "MediatorCommandInitiator.h"

void CMediatorCommandInitiator::CancelCommand(TUid, TUid, TInt) {}
CMediatorCommandInitiator * CMediatorCommandInitiator::NewL( MMediatorCommandResponseObserver *) { return new( ELeave )CMediatorCommandInitiator(); }
CMediatorCommandInitiator::CMediatorCommandInitiator() {}
CMediatorCommandInitiator::~CMediatorCommandInitiator() {}

extern TUid issueCommandDomain;
extern TUid issueCommandCategory;
extern TInt issueCommandCommandId;
extern TVersion issueCommandVersion;
extern HBufC8* issueCommandData;
extern TInt issueCommandReturnValue;
TInt CMediatorCommandInitiator::IssueCommand(TUid aDomain, TUid aCategory, TInt aCmdId, TVersion aVersion, const TDesC8 & aData) 
    {
    issueCommandDomain = aDomain;
    issueCommandCategory = aCategory;
    issueCommandCommandId = aCmdId;
    issueCommandVersion = aVersion;
    issueCommandData = aData.Alloc();
    if ( issueCommandData == NULL )
        {
        EUNIT_FAIL_TEST( "issueCommandData = aData.Alloc();" );
        }
    return issueCommandReturnValue; 
    }

#include "MediatorEventProvider.h"


CMediatorEventProvider * CMediatorEventProvider::NewL() { return new( ELeave )CMediatorEventProvider(); } 
CMediatorEventProvider::CMediatorEventProvider() {}
CMediatorEventProvider::~CMediatorEventProvider() {}
extern TUid raiseEventDomain;
extern TUid raiseEventCategory;
extern TInt raiseEventEventId;
extern TVersion raiseEventVersion;
extern HBufC8* raiseEventData;
TInt CMediatorEventProvider::RaiseEvent( TUid aDomain, TUid aCategory, TInt aEventId, TVersion aVersion, const TDesC8 & aData) 
    { 
    raiseEventDomain = aDomain;
    raiseEventCategory = aCategory;
    raiseEventEventId = aEventId;
    raiseEventVersion = aVersion;
    raiseEventData = aData.Alloc();
    if ( raiseEventData == NULL )
        {
        EUNIT_FAIL_TEST( "raiseEventData = aData.Alloc() failed" );
        }
    return KErrNone; 
    }

extern TInt mediatorRegisterationReturnValue;
extern TUid domain;
extern TUid category;
extern MediatorService::REventList eventList;
TInt CMediatorEventProvider::RegisterEvent( TUid aDomain, TUid aCategory, const RArray<MediatorService::TEvent>& events ) 
    { 
    domain = TUid(aDomain);
    category = TUid(aCategory);
    for ( TInt i = 0; i < events.Count(); i++ )
        {
        eventList.Append(events[i]);
        }
    return mediatorRegisterationReturnValue; 
    }

TInt CMediatorEventProvider::UnregisterEvent(class TUid, class TUid, class RArray<class MediatorService::TEvent> const &) { return 0; }

#include "cphonelogger.h"

CPhoneMethodLogger::CPhoneMethodLogger(
        TPhoneUILoggingComponent /*aLoggingComponent*/,
        TPtrC /*aString*/ )
    {
    }

CPhoneMethodLogger::~CPhoneMethodLogger()
    {
    }

#include <coemain.h>
CCoeStatic::~CCoeStatic() {}
CCoeStatic::CCoeStatic() {}
CCoeStatic::CCoeStatic( TUid, TScope ) { }
void CCoeStatic::CCoeStatic_Reserved1() {}
void CCoeStatic::CCoeStatic_Reserved2() {}

#include "CPhoneMediatorSender.h"
extern CPhoneMediatorSender* coeEnvMediatorSenderInstance;
CCoeStatic* CCoeEnv::Static( TUid ) { return coeEnvMediatorSenderInstance; }
