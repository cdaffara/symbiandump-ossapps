/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: remconcoreapitarget stub for testing MpMediaKeyHandler
*
*/

#include <e32debug.h>

#include "stub/inc/remconcoreapitargetobserver.h"
#include "stub/inc/remconcoreapitarget.h"


int gCRemConCoreApiTargetCount = 0;
bool gCRemConCoreApiTargetNewLLeave = false;
int gCRemConCoreApiTargetSendResponseCount = 0;


//static functions start

int CRemConCoreApiTarget::getCount()
{
    return gCRemConCoreApiTargetCount;
}

void CRemConCoreApiTarget::setNewLLeave()
{
    gCRemConCoreApiTargetNewLLeave = true;
}

void CRemConCoreApiTarget::NewLLeaveIfDesiredL()
{
    if (gCRemConCoreApiTargetNewLLeave)
    {
        RDebug::Print(_L("CRemConCoreApiTarget::NewLLeaveIfDesiredL Leave"));
        gCRemConCoreApiTargetNewLLeave = false;
        User::Leave(KErrGeneral);
    }
}

int CRemConCoreApiTarget::getSendResponseCount()
{
    return gCRemConCoreApiTargetSendResponseCount;
}

//static functions end


CRemConCoreApiTarget::CRemConCoreApiTarget()
    : CActive(EPriorityStandard),
      iClientStatus(NULL)
{
    gCRemConCoreApiTargetCount++;
}

CRemConCoreApiTarget::~CRemConCoreApiTarget()
{
    gCRemConCoreApiTargetCount--;
    gCRemConCoreApiTargetSendResponseCount = 0;
}

CRemConCoreApiTarget* CRemConCoreApiTarget::NewL(CRemConInterfaceSelector& /*aInterfaceSelector*/, MRemConCoreApiTargetObserver& /*aObserver*/)
{
    RDebug::Print(_L("stub CRemConCoreApiTarget::NewL"));
    CRemConCoreApiTarget::NewLLeaveIfDesiredL();
    CRemConCoreApiTarget* self = new(ELeave) CRemConCoreApiTarget();
    return self;
}

void CRemConCoreApiTarget::PausePlayFunctionResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::PausePlayFunctionResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
  
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::PausePlayFunctionResponse"));
}

void CRemConCoreApiTarget::PlayResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::PlayResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::PlayResponse"));
}

void CRemConCoreApiTarget::StopResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::StopResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::StopResponse"));
}

void CRemConCoreApiTarget::PauseResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::PauseResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::PauseResponse"));
}

void CRemConCoreApiTarget::RewindResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::RewindResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::RewindResponse"));
}

void CRemConCoreApiTarget::FastForwardResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::FastForwardResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::FastForwardResponse"));
}

void CRemConCoreApiTarget::BackwardResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::BackwardResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::BackwardResponse"));
}

void CRemConCoreApiTarget::ForwardResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::ForwardResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::ForwardResponse"));
}

void CRemConCoreApiTarget::VolumeUpResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::VolumeUpResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::VolumeUpResponse"));
}

void CRemConCoreApiTarget::VolumeDownResponse(TRequestStatus& aStatus, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::VolumeDownResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/
    
    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::VolumeDownResponse"));
}

void CRemConCoreApiTarget::SendResponse(TRequestStatus& aStatus, TRemConCoreApiOperationId /*aOperationId*/, TInt /*aError*/)
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::SendResponse"));
    
    iClientStatus = &aStatus;
    gCRemConCoreApiTargetSendResponseCount++;
    
    // stub should not be async
    /*TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();*/

    RunL();
    
    RDebug::Print(_L("<<CRemConCoreApiTarget::SendResponse"));
}

void CRemConCoreApiTarget::DoCancel()
{
}

void CRemConCoreApiTarget::RunL()
{
    RDebug::Print(_L(">>CRemConCoreApiTarget::RunL"));
    User::RequestComplete(iClientStatus, KErrNone);
    RDebug::Print(_L("<<CRemConCoreApiTarget::RunL"));
}

//end of file
