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

#include <logcli.h>
#include <logview.h>
#include <logclientchangeobserver.h>

#include "logclient_stubs_helper.h"

bool stubAsyncCallPossible = true;
int stubViewCount = 1;
int stubLeaveWithError = KErrNone;
int stubViewFlagsValue = 0;
QString stubLastCalledFunc;

// ----------------------------------------------------------------------------
// LogClientStubsHelper
// ----------------------------------------------------------------------------
//
void LogClientStubsHelper::reset()
{
    stubAsyncCallPossible = true;
    stubLeaveWithError = KErrNone;
    stubViewFlagsValue = 0;
    stubLastCalledFunc.clear();
}

void LogClientStubsHelper::createEvents(int /*numEvents*/)
{

}

void LogClientStubsHelper::setStubAsyncCallPossible(bool asyncPossible)
{
    stubAsyncCallPossible = asyncPossible;
}

void LogClientStubsHelper::setViewCount(int viewCount)
{
    stubViewCount = viewCount;
}

void LogClientStubsHelper::setLeaveWithError(int error)
{
    stubLeaveWithError = error;
}

int LogClientStubsHelper::stubViewFlags()
{
    return stubViewFlagsValue;
}

QString LogClientStubsHelper::lastCalledFunc()
{
    return stubLastCalledFunc;
}

// ----------------------------------------------------------------------------
// CLogActive
// ----------------------------------------------------------------------------
//
CLogActive::~CLogActive()
{
}

CLogActive::CLogActive(TInt aPriority) : CActive( aPriority )
{

}
void CLogActive::DoCancel()
{

}
void CLogActive::RunL()
{

}
void CLogActive::DoComplete(TInt&)
{

}

// ----------------------------------------------------------------------------
// CLogBase
// ----------------------------------------------------------------------------
//
CLogBase::CLogBase(TInt aPriority) : CLogActive( aPriority )
{

}
CLogBase::~CLogBase()
{

}

TInt CLogBase::GetString(TDes& aString, TInt /*aId*/) const
{
    aString.Copy( _L("dummy_stub_string") );
    return KErrNone;
}

void CLogBase::GetEvent(CLogEvent& /*aEvent*/, TRequestStatus& /*aStatus*/)
{

}
void CLogBase::AddEvent(CLogEvent& /*aEvent*/, TRequestStatus& /*aStatus*/)
{

}
void CLogBase::ChangeEvent(const CLogEvent& /*aEvent*/, TRequestStatus& /*aStatus*/)
{

}
void CLogBase::DeleteEvent(TLogId /*aId*/, TRequestStatus& /*aStatus*/)
{

}
void CLogBase::DoRunL()
{

}
void CLogBase::CLogBase_Reserved1()
{

}

// ----------------------------------------------------------------------------
// CLogClient
// ----------------------------------------------------------------------------
//
CLogClient* CLogClient::NewL(RFs& aFs, TInt aPriority)
{
    User::LeaveIfError(stubLeaveWithError);
    CLogClient* self = new ( ELeave ) CLogClient(aFs, aPriority);
    return self;
}

CLogClient::CLogClient(RFs& aFs, TInt aPriority) : CLogBase(aPriority), iFs( aFs )
{
}
CLogClient::~CLogClient()
{
    
}

void CLogClient::SetGlobalChangeObserverL(MLogClientChangeObserver* /*aObserver*/)
{

}

TInt CLogClient::GetString(TDes& aString, TInt /*aId*/) const
{
    aString.Copy( _L("dummy_stub") );
    return KErrNone;
}

void CLogClient::AddEvent(CLogEvent& /*aEvent*/, TRequestStatus& /*aStatus*/)
{

}
void CLogClient::GetEvent(CLogEvent& /*aEvent*/, TRequestStatus& /*aStatus*/)
{

}
void CLogClient::ChangeEvent(const CLogEvent& /*aEvent*/, TRequestStatus& /*aStatus*/)
{

}
void CLogClient::DeleteEvent(TLogId /*aId*/, TRequestStatus& aStatus)
{
    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
}

void CLogClient::DoRunL()
{

}
void CLogClient::DoCancel()
{

}

void CLogClient::ClearLog(const TTime& /*aDate*/, TRequestStatus& aStatus)
{
    stubLastCalledFunc = "ClearLog";
    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
}

void CLogClient::ClearLog(TInt /*aRecentList*/, TRequestStatus& aStatus)
{
    stubLastCalledFunc = "ClearLogRecentList";
    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
}

void CLogClient::CLogBase_Reserved1()
{

}

// ----------------------------------------------------------------------------
// CLogFilterList
// ----------------------------------------------------------------------------
//
CLogFilterList::CLogFilterList() : CArrayPtrFlat( 4 )
{

}

// ----------------------------------------------------------------------------
// CLogFilter
// ----------------------------------------------------------------------------
//
CLogFilter* CLogFilter::NewL()
{
    return new ( ELeave ) CLogFilter;
}
CLogFilter::~CLogFilter()
{

}

CLogFilter::CLogFilter()
{

}

// ----------------------------------------------------------------------------
// CLogView
// ----------------------------------------------------------------------------
//
CLogView::~CLogView()
{
    delete iEvent;
}

TBool CLogView::FirstL(TRequestStatus& aStatus)
{
    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
    return ETrue;
}

TBool CLogView::NextL(TRequestStatus& aStatus)
{
    if ( stubAsyncCallPossible ){
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
    }
    return stubAsyncCallPossible;
}

TInt CLogView::CountL()
{
    return stubViewCount;
}

CLogView::CLogView(CLogClient& aClient, TInt aPriority) 
    : CLogActive( aPriority ), iClient( aClient )
{
    TRAP_IGNORE( iEvent = CLogEvent::NewL() )
}

void CLogView::DoRunL()
{

}

void CLogView::DoCancel()
{

}

void CLogView::SetFlagsL(TLogFlags aFlags)
{
    stubViewFlagsValue = aFlags;
}

// ----------------------------------------------------------------------------
// CLogViewRecent
// ----------------------------------------------------------------------------
//
CLogViewRecent* CLogViewRecent::NewL(CLogClient& aClient, TInt aPriority)
{
return new ( ELeave ) CLogViewRecent(aClient, aPriority );
}
CLogViewRecent* CLogViewRecent::NewL(
    CLogClient& aClient, MLogViewChangeObserver& /*aObserver*/, TInt aPriority)
{
    return new ( ELeave ) CLogViewRecent(aClient, aPriority );
}
CLogViewRecent::~CLogViewRecent()
{

}

CLogViewRecent::CLogViewRecent(CLogClient& aClient, TInt aPriority) 
    : CLogView( aClient, aPriority )
{

}
TBool CLogViewRecent::SetRecentListL(TLogRecentList aList, TRequestStatus& aStatus)
{
    iRecentList = aList;
    if ( stubAsyncCallPossible ){
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
    }
    return stubAsyncCallPossible;
}
TBool CLogViewRecent::SetRecentListL(
    TLogRecentList aList, const CLogFilter& /*aFilter*/, TRequestStatus& aStatus)
{
    iRecentList = aList;
    if (  stubAsyncCallPossible ){
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
    }
    return stubAsyncCallPossible;
}
TBool CLogViewRecent::SetRecentListL(
    TLogRecentList aList, const CLogFilterList& /*aFilterList*/, TRequestStatus& aStatus)
{
    iRecentList = aList;
    if ( stubAsyncCallPossible ){
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
    }
    return stubAsyncCallPossible;
}

void CLogViewRecent::DoRunL()
{

}

TBool CLogViewRecent::DuplicatesL(CLogViewDuplicate& /*aView*/, TRequestStatus& aStatus)
{
    if ( stubAsyncCallPossible ){
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
    }
    return stubAsyncCallPossible;
}

TBool CLogViewRecent::DuplicatesL(CLogViewDuplicate &/*aView*/, const CLogFilter &/*aFilter*/, TRequestStatus &aStatus)
{
    if ( stubAsyncCallPossible ){
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
    }
    return stubAsyncCallPossible;
}

void CLogViewRecent::RemoveL(TLogId /*aId*/)
{
    
}

// ----------------------------------------------------------------------------
// CLogViewEvent
// ----------------------------------------------------------------------------
//
CLogViewEvent* CLogViewEvent::NewL(CLogClient& aClient, TInt aPriority)
{
    CLogViewEvent* self = new (ELeave) CLogViewEvent(aClient, aPriority);
    return self;
}
CLogViewEvent* CLogViewEvent::NewL(
    CLogClient& aClient, MLogViewChangeObserver& /*aObserver*/, TInt aPriority)
{
    CLogViewEvent* self = new (ELeave) CLogViewEvent(aClient, aPriority);
    return self;
}
CLogViewEvent::~CLogViewEvent()
{

}
TBool CLogViewEvent::SetFilterL(const CLogFilterList& /*aFilterList*/, TRequestStatus& aStatus)
{
    if ( stubAsyncCallPossible ){
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
    }
    return stubAsyncCallPossible;
}
CLogViewEvent::CLogViewEvent(CLogClient& aClient, TInt aPriority) : 
    CLogView(aClient, aPriority)
{
}
    
// ----------------------------------------------------------------------------
// CLogViewDuplicate
// ----------------------------------------------------------------------------
//
CLogViewDuplicate* CLogViewDuplicate::NewL(
    CLogClient& aClient, TInt aPriority)
{
    return new ( ELeave ) CLogViewDuplicate(aClient, aPriority );
}

CLogViewDuplicate* CLogViewDuplicate::NewL(
        CLogClient& aClient, MLogViewChangeObserver& /*aObserver*/, TInt aPriority)
{
    return new ( ELeave ) CLogViewDuplicate(aClient, aPriority );
}
    
CLogViewDuplicate::~CLogViewDuplicate()
{

}

CLogViewDuplicate::CLogViewDuplicate(CLogClient& aClient, TInt aPriority) :
    CLogView(aClient, aPriority)
{

}

// ----------------------------------------------------------------------------
// CLogEvent
// ----------------------------------------------------------------------------
//
CLogEvent* CLogEvent::NewL()
{
    CLogEvent* self = new ( ELeave ) CLogEvent;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

void CLogEvent::ConstructL()
{
    iNumber = HBufC::NewL( 200 );
    iDirection = HBufC::NewL( 200 );
    iRemoteParty = HBufC::NewL( 200 );
}
CLogEvent::~CLogEvent()
{
    delete iNumber;
    delete iDirection;
    delete iData;
    delete iRemoteParty;
}
CLogEvent::CLogEvent()
{

}

void CLogEvent::SetDataL(const TDesC8& aData)
{
    HBufC8* data = aData.AllocL();
    delete iData;
    iData = data;
}

// ----------------------------------------------------------------------------
// MLogClientChangeObserver
// ----------------------------------------------------------------------------
//
void MLogClientChangeObserver::MLogClientChangeObserver_Reserved1()
{

}
void MLogClientChangeObserver::MLogClientChangeObserver_Reserved2()
{

}
