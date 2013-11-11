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
* Description:   Logging utility application for MC Photos
*
*/



#include <avkon.hrh>
#include <aknnotewrappers.h> 

#include <glxlogger.h>

#include "glxlogging.pan"
#include "glxloggingappui.h"
#include "glxloggingapp.hrh"


// Constants
namespace
    {

const TInt KLoggingDriveLetterLength = 3;

_LIT(KColonBackSlash, ":\\");    
      
#ifdef __WINSCW__
    // log to the c: drive on the emulator
    _LIT(KGlxLoggingFolderDrive, "c");
#else
    // log to the root of the removable media card on hardware
    _LIT(KGlxLoggingFolderDrive, "e");
#endif
    
    // write logged data to the file every 0.5s
    const TInt KGlxLoggingIntervalMicroSec = 500000;
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
void CGlxLoggingAppUi::ConstructL()
    {
    BaseConstructL();

    // we dont want to respond to shutdown events
    iEikonEnv->SetRespondsToShutdownEvent( EFalse );
    
    // Log manager
    iLogManager.CreateL();
      
    // Create and start the timer
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iTimer->Start( KGlxLoggingIntervalMicroSec,
        KGlxLoggingIntervalMicroSec, TCallBack( TimerCallback, this ) );
    }

// -----------------------------------------------------------------------------
// CGlxLoggingAppUi
// -----------------------------------------------------------------------------
CGlxLoggingAppUi::CGlxLoggingAppUi()                              
    {
    // delete timer
    delete iTimer;
    // release the chunks
    iLogManager.Release();
    }

// -----------------------------------------------------------------------------
// ~CGlxLoggingAppUi
// -----------------------------------------------------------------------------
CGlxLoggingAppUi::~CGlxLoggingAppUi()
    {
    // try to write the remainder of the log
    DoLog();
    }

// -----------------------------------------------------------------------------
// TimerCallback
// -----------------------------------------------------------------------------
TInt CGlxLoggingAppUi::TimerCallback( TAny* aParam )
    {
    return reinterpret_cast<CGlxLoggingAppUi*>( aParam )->DoLog();
    }

// -----------------------------------------------------------------------------
// DoLog
// -----------------------------------------------------------------------------
TInt CGlxLoggingAppUi::DoLog()
    {
    // Timer has called back so update the log file with all the entries
    // that have been written to the chunks in the meantime
    // just try to write all, nothing to do on leave
    TBuf<KLoggingDriveLetterLength> driveLetter;
    driveLetter.Append(KGlxLoggingFolderDrive);
    driveLetter.Append(KColonBackSlash);
    TRAP_IGNORE( iLogManager.CommitToFileL( driveLetter ) );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// HandleCommandL. handle any menu commands
// -----------------------------------------------------------------------------
void CGlxLoggingAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
        default:
            Panic(EGlxLoggingBasicUi);
            break;
        }
    }



