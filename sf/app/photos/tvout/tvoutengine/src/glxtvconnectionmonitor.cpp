/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Monitors the Tv Out Connection
*
*/



//  CLASS HEADER
#include "glxtvconnectionmonitor.h"

//  EXTERNAL INCLUDES
#include <AccPolGenericIdDefinitions.h>
#include <AccPolSubblockNameArray.h>

//  INTERNAL INCLUDES
#include <glxlog.h>
#include <glxtracer.h>
#include <glxpanic.h>
#include "glxtv.h"

//-----------------------------------------------------------------------------
// Return new object
//-----------------------------------------------------------------------------
//
CGlxTvConnectionMonitor* CGlxTvConnectionMonitor::NewL(
                               MGlxTvConnectionObserver& aConnectionObserver ) 
    {
    TRACER("CGlxTvConnectionMonitor::NewL");
    CGlxTvConnectionMonitor* self = new (ELeave) 
                CGlxTvConnectionMonitor( aConnectionObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
//
CGlxTvConnectionMonitor::~CGlxTvConnectionMonitor()
    {
    TRACER("~CGlxTvConnectionMonitor");
    Cancel();
    iTvAccCon.CloseSubSession();
    iTvAccServer.Disconnect();
    }


//-----------------------------------------------------------------------------
// Default C++ constructor
//-----------------------------------------------------------------------------
//
CGlxTvConnectionMonitor::CGlxTvConnectionMonitor(
		MGlxTvConnectionObserver& aConnectionObserver) :
	CActive(EPriorityStandard), iConnectionObserver(aConnectionObserver),
			iIsTvOutConnected(EFalse), iIsHDMIConnected(EFalse),
			iIsHeadSetConnected(EFalse)

	{
	TRACER("CGlxTvConnectionMonitor");
	CActiveScheduler::Add(this);
	}


//-----------------------------------------------------------------------------
// Symbian second phase constructor
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::ConstructL()
    {
    TRACER("CGlxTvConnectionMonitor::ConstructL");
    User::LeaveIfError(iTvAccServer.Connect());
	User::LeaveIfError(iTvAccCon.CreateSubSession(iTvAccServer));

	UpdateConnectionStatusL();
	IssueRequest();  
    }

//-----------------------------------------------------------------------------
// From class CActive.
// Receive notification of change in the connection state
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::RunL()
    {
    TRACER("CGlxTvConnectionMonitor::RunL");
    // Check for errors
    User::LeaveIfError( iStatus.Int() );
    // Notify observers
    IssueNotificationL();
    // Request the next event
    IssueRequest();  
    }


//-----------------------------------------------------------------------------
// From class CActive.
// DoCancel
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::DoCancel()
    {
    TRACER("CGlxTvConnectionMonitor::DoCancel");
    iTvAccCon.CancelGetAccessoryConnectionStatus();
    iTvAccCon.CancelNotifyAccessoryConnectionStatusChanged();
    }



//-----------------------------------------------------------------------------
// From class CActive.
// RunError
//-----------------------------------------------------------------------------
//
TInt CGlxTvConnectionMonitor::RunError(TInt aError)
    {
    GLX_LOG_INFO1("CGlxTvConnectionMonitor::RunError - %d", aError);
    return KErrNone;
    }

//-----------------------------------------------------------------------------
// Is the TV out cable connected
// IsConnected
//-----------------------------------------------------------------------------
//
TBool CGlxTvConnectionMonitor::IsConnected() const
    {
    TRACER("CGlxTvConnectionMonitor::IsConnected");
    GLX_LOG_INFO1("CGlxTvConnectionMonitor::IsConnected iIsTvOutConnected=%d",iIsTvOutConnected);
    return iIsTvOutConnected;
    }

//-----------------------------------------------------------------------------
// Is the TV out cable connected
// IsConnected
//-----------------------------------------------------------------------------
//
TBool CGlxTvConnectionMonitor::IsHDMIConnected() const
    {
    TRACER("CGlxTvConnectionMonitor::IsHDMIConnected");
    GLX_LOG_INFO1("CGlxTvConnectionMonitor::IsHDMIConnected iIsHDMIConnected=%d",iIsHDMIConnected);
    return iIsHDMIConnected;
    }

//-----------------------------------------------------------------------------
// Request accessory server events
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::IssueRequest()
    {
    TRACER("CGlxTvConnectionMonitor::IssueRequest");
    if (!IsActive()) // required for testing
        {
        iTvAccCon.NotifyAccessoryConnectionStatusChanged(iStatus,iCurrentAccArray);
        SetActive(); 
        }
    }

//-----------------------------------------------------------------------------
// Sends notification to observers if TV Out is connected
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::IssueNotificationL()
	{
	TRACER("CGlxTvConnectionMonitor::IssueNotificationL");
	TBool previousTvState = iIsTvOutConnected;
	TBool previousHDMIState = iIsHDMIConnected;
	TBool previousHeadSetState = iIsHeadSetConnected;
	TTvChangeType changeType = EDisconnected;
	
	// Update the connection status
	UpdateConnectionStatusL();

	// trigger tvstatus change only when there is actually a change the in the connection and not 
	// for spurious events
	if (previousTvState != iIsTvOutConnected)
		{
		changeType = iIsTvOutConnected == EFalse ? EDisconnected : ETvConnectionChanged;
		GLX_LOG_INFO1("CGlxTvConnectionMonitor::IssueNotificationL - TVOut Connection Changed %d", changeType);
		iConnectionObserver.HandleTvConnectionStatusChangedL(changeType);	
		}
	else if (previousHDMIState != iIsHDMIConnected)
		{
		changeType = iIsHDMIConnected == EFalse ? EDisconnected : ETvConnectionChanged;
		GLX_LOG_INFO1("CGlxTvConnectionMonitor::IssueNotificationL - HDMI Connection Changed %d", changeType);
		iConnectionObserver.HandleTvConnectionStatusChangedL(changeType);	
		}
	else if (previousHeadSetState != iIsHeadSetConnected)
		{
		changeType = iIsHeadSetConnected == EFalse ? EDisconnected : ETvConnectionChanged;
		GLX_LOG_INFO1("CGlxTvConnectionMonitor::IssueNotificationL - Headset Connection Changed %d", changeType);
		iConnectionObserver.HandleTvConnectionStatusChangedL(changeType);	
		}
	}

//-----------------------------------------------------------------------------
// UpdateConnectionStatusL()
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::UpdateConnectionStatusL()
    {
    TRACER("CGlxTvConnectionMonitor::UpdateConnectionStatusL()");

    GLX_LOG_INFO3("CGlxTvConnectionMonitor::UpdateConnectionStatusL previousTvState = %d , previousHDMIState = %d iIsHeadSetConnected=%d",
            iIsTvOutConnected,iIsHDMIConnected,iIsHeadSetConnected);

    // reset the states
    iIsHDMIConnected = EFalse;
    iIsTvOutConnected = EFalse;
    iIsHeadSetConnected = EFalse;

    //gets the TV status in to the iCurrentAccArray and haves the Latest Accesory in 0-index
    User::LeaveIfError(iTvAccCon.GetAccessoryConnectionStatus(iCurrentAccArray));

    CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
    CleanupStack::PushL(nameArray);

    //  Get the number of currently connected accessories
    //  Loop through each connected accessory looking for
    //  the desired capabilities
    //
    const TInt count = iCurrentAccArray.Count();
    GLX_LOG_INFO1("CGlxTvConnectionMonitor::UpdateConnectionStatusL current count value is %d",count);
    for (TInt index = 0; index < count; index++)
        {
        const TAccPolGenericID genId = iCurrentAccArray.GetGenericIDL(index);
        //  Check if this connected accessory supports TV-Out capabality.
        GLX_LOG_INFO1("CGlxTvConnectionMonitor::UpdateConnectionStatusL current index value is %d",index);
        GLX_LOG_INFO("CGlxTvConnectionMonitor::UpdateConnectionStatusL Video Accessory Supported");
        TAccPolNameRecord nameRecord;
        nameRecord.SetNameL(KAccVideoOut);

        if (genId.DeviceTypeCaps( KDTAVDevice ) &&
                genId.PhysicalConnectionCaps( KPCHDMI ))
            {
            TAccValueTypeTInt value;
            TInt err = iTvAccCon.GetValueL(genId, nameRecord, value);
            if (err ==KErrNone)
                {
                if (value.iValue == EAccVideoOutEHDMI)
                    {
                    GLX_LOG_INFO("CGlxTvConnectionMonitor::UpdateConnectionStatusL HDMI is connected");
                    iIsHDMIConnected = ETrue;
                    }
                }
            }
        else if (genId.DeviceTypeCaps( KDTAVDevice ) &&
                genId.PhysicalConnectionCaps( KPCWired ))
            {
            TAccValueTypeTInt value;
            TInt err = iTvAccCon.GetValueL(genId, nameRecord, value);
            if (err ==KErrNone)
                {
                if (value.iValue == EAccVideoOutCVideo)
                    {
                    GLX_LOG_INFO("CGlxTvConnectionMonitor::UpdateConnectionStatusL Tv is connected");
                    iIsTvOutConnected = ETrue;
                    }
                }
            }
		else if (genId.DeviceTypeCaps(KDTHeadset)
				&& genId.PhysicalConnectionCaps(KPCWired))
			{
			GLX_LOG_INFO("CGlxTvConnectionMonitor::UpdateConnectionStatusL HeadSet connect");
			iIsHeadSetConnected = ETrue;
			}
		}
    CleanupStack::PopAndDestroy(nameArray);

    GLX_LOG_INFO3("CGlxTvConnectionMonitor::UpdateConnectionStatusL CurrentTvState = %d , CurrentHDMIState = %d, iIsHeadSetConnected=%d",
            iIsTvOutConnected,iIsHDMIConnected,iIsHeadSetConnected);
    }
