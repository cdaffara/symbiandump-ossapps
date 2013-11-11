/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Mylocation engine source implementation for calender db path observation.
 *
 */

#include <calendernotification.h>

_LIT( KCalendarDbPath, ":\\private\\10003a5b\\" );

const TInt KMaxFilePtahSize = 256;

// -----------------------------------------------------------------------------
// CMyLocationsEngine::NewL()
// -----------------------------------------------------------------------------
//
CCalenderNotification* CCalenderNotification::NewL(MNotifyChange* aNotifyChange)
{
    CCalenderNotification* self = CCalenderNotification::NewLC(aNotifyChange);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::NewLC()
// -----------------------------------------------------------------------------
//
CCalenderNotification* CCalenderNotification::NewLC(
        MNotifyChange* aNotifyChange)
{
    CCalenderNotification* self = new (ELeave) CCalenderNotification(
            aNotifyChange);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCalenderNotification::ConstructL()
{
    CActiveScheduler::Add(this);
    User::LeaveIfError(iFsession.Connect());

}


// -----------------------------------------------------------------------------
// CMyLocationsEngine::CheckCalenderDbFileStructure()
// Observes the calendar db file structure changes.
// -----------------------------------------------------------------------------
//
void CCalenderNotification::CheckCalenderDbFileStructure( TChar aDrive )
{
    TBuf<KMaxFilePtahSize> filePath;
    filePath.Append( aDrive );
    filePath.Append( KCalendarDbPath );
    iFsession.NotifyChange(ENotifyEntry, iStatus, filePath );
    SetActive();
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::CCalenderNotification()
// Default constructor .
// -----------------------------------------------------------------------------
//
CCalenderNotification::CCalenderNotification(MNotifyChange* aNotifyChange) :
    CActive(EPriorityStandard), iNotifyChange(*aNotifyChange)
{
}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::~CCalenderNotification()
// default destuctor.
// -----------------------------------------------------------------------------
//
CCalenderNotification::~CCalenderNotification()
{
    Cancel();
    iFsession.Close();
}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::RunL()
// Assyncronous request handler , on completion of notification
// -----------------------------------------------------------------------------
//
void CCalenderNotification::RunL()
{
    TInt status;
    iNotifyChange.NotifyChangeL( status );
}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::DoCancel()
// Handels the error condition on assynchronous request
// -----------------------------------------------------------------------------
//
void CCalenderNotification::DoCancel()
{
    iFsession.NotifyChangeCancel();
}

//End of file

