/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the class CEtelIncomingCallMonitor
*
*/

#include "etelincomingcallmonitor.h"
#include <etelmmcs.h>
#include <QDebug>

// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CEtelIncomingCallMonitor::~CEtelIncomingCallMonitor( )
    {
    Cancel();
    iLine.Close();
    iServer.Close();
    }

// ---------------------------------------------------------------------------
// Starts the monitor.
// ---------------------------------------------------------------------------
//
void CEtelIncomingCallMonitor::StartMonitoring()
    {
    if ( !IsActive() )
        {
        iCallName.Zero();
        iLine.NotifyIncomingCall( iStatus, iCallName );
        SetActive();
        }

    }
// ---------------------------------------------------------------------------
// From CActive.
// Handles line status change notifying.
// ---------------------------------------------------------------------------
//
void CEtelIncomingCallMonitor::RunL()
    {
    
    qDebug () << "CEtelIncomingCallMonitor::RunL<<";
    if ( iStatus == KErrNone )
        {
        RMobileCall mobilecall;
        TInt ret = mobilecall.OpenExistingCall(iLine,iCallName);
        qDebug() << "CEtelIncomingCallMonitor::RunL_OpenExistingCall_ret:" << ret;
        ret = mobilecall.AnswerIncomingCall();
        qDebug() << "CEtelIncomingCallMonitor::RunL_AnswerIncomingCall_ret:" << ret;
        mobilecall.Close();
        // Continue monitoring
        StartMonitoring();        
        }
    qDebug () << "CEtelIncomingCallMonitor::RunL>>";
    }

// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CEtelIncomingCallMonitor::DoCancel()
    {
    if ( iStatus == KRequestPending )
        {
        iLine.NotifyIncomingCallCancel();
        }
    }

// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CEtelIncomingCallMonitor::CEtelIncomingCallMonitor(
            /*RMobileLine& aLine,
            TInt aLineId */) : 
            CActive( EPriorityStandard + 2 )
            
    {
    CActiveScheduler::Add( this );
    const TInt KNbrOfMessageSlots = 128;
    TInt phoneCount( 0 );
    

    int errorCode = iServer.Connect( KNbrOfMessageSlots );
        
    RMobilePhone mobilePhone;
    errorCode = iServer.EnumeratePhones( phoneCount );
    errorCode = iServer.GetPhoneInfo( phoneCount-1, iPhoneInfo );     
    errorCode = mobilePhone.Open( iServer, iPhoneInfo.iName );
    int error = iLine.Open( mobilePhone,_L("Voice1"));
    }

// End of File
