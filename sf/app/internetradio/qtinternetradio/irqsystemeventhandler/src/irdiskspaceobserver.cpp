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
#include <pathinfo.h>
#include "irdiskspaceobserver.h"
#include "irsystemeventobserverinterface.h"

const TDriveNumber KQtIRDefaultDrive = EDriveC;

const TUint KDriveAValue = 'A';
const TUint KDriveZValue = 'Z';
const TUint KDriveaValue = 'a';
const TUint KDrivezValue = 'z';

static TDriveNumber phoneMemRootDrive();

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CIRDiskSpaceObserver* CIRDiskSpaceObserver::NewL(MIRDiskSpaceObserverInterface* aObserver)
{
    CIRDiskSpaceObserver* self = NewLC(aObserver);   
    CleanupStack::Pop(self);
    return self;    
}

CIRDiskSpaceObserver* CIRDiskSpaceObserver::NewLC(MIRDiskSpaceObserverInterface* aObserver)
{     
    CIRDiskSpaceObserver* self = new (ELeave) CIRDiskSpaceObserver(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// Second stage constructor
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceObserver::ConstructL()
{
    CActiveScheduler::Add(this);
    User::LeaveIfError(iFs.Connect());
}

// ---------------------------------------------------------------------------
// C++ Constructor
// ---------------------------------------------------------------------------
//
CIRDiskSpaceObserver::CIRDiskSpaceObserver( MIRDiskSpaceObserverInterface* aObserver )
                              : CActive( CActive::EPriorityStandard ),       
                                iObserver( aObserver ),
                                iCriticalLevel(0)
{
}
            
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//            
CIRDiskSpaceObserver::~CIRDiskSpaceObserver()
{
    Cancel();
    iFs.Close();    
}

// ---------------------------------------------------------------------------
// Critical Level
// ---------------------------------------------------------------------------
//
TInt64 CIRDiskSpaceObserver::CriticalLevel() const
{
    if(IsActive())
    {
        return iCriticalLevel;
    }
    else
    {
        return -1;
    }    
}

// ---------------------------------------------------------------------------
// Start Monitoring
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceObserver::Start(const TInt64 aCriticalLevel)
{
    Cancel();
    iCriticalLevel = aCriticalLevel;
    Run();
}


// ---------------------------------------------------------------------------
// From class CActive. 
// Cancels the pending request.
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceObserver::DoCancel() 
{    
    iFs.NotifyDiskSpaceCancel(iStatus);
}


// ---------------------------------------------------------------------------
// From class CActive. 
// Invoked when the observed disk's free disk space has run below
// the supplied critical level.
// Notifies the observer and regenerates the request to RFs' NotifyDiskSpace.
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceObserver::RunL()
{       
    Run();   
}
    

// ---------------------------------------------------------------------------
// Returns true if below the critical level in defined disk, otherwise false
// ---------------------------------------------------------------------------
//
TBool CIRDiskSpaceObserver::IsBelowCriticalLevel(const TInt64 aCriticalLevel) const
{
    TVolumeInfo volInfo;
    if(KErrNone == iFs.Volume(volInfo, KQtIRDefaultDrive))
    {
        if(volInfo.iFree < aCriticalLevel)
        {
            return ETrue;
        }
    }
    
    return EFalse;
}

void CIRDiskSpaceObserver::Run()
{
    if( iObserver && IsBelowCriticalLevel(iCriticalLevel))
    {
        iObserver->notifyLowDiskSpace((qint64)iCriticalLevel);
    }
        
    iFs.NotifyDiskSpace(iCriticalLevel, phoneMemRootDrive(), iStatus);
    SetActive(); 
}

TDriveNumber phoneMemRootDrive()
{
    TParsePtrC phoneMemoryRootPath( PathInfo::PhoneMemoryRootPath() );
    TPtrC drive( phoneMemoryRootPath.Drive() );
    TUint driveValue = drive[0];
    TUint delta = 0;
    if( driveValue >= KDriveAValue &&
        driveValue <= KDriveZValue)
    {
        delta = driveValue - KDriveAValue;
    }
    else if( driveValue >= KDriveaValue &&
            driveValue <= KDrivezValue)
    {
        delta = driveValue - KDriveaValue;
    }
    else
    {
        return KQtIRDefaultDrive;
    }
    
    return static_cast<TDriveNumber>(EDriveA + delta);
}
