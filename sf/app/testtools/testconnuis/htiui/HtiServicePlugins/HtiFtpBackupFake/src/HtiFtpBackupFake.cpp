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
* Description:  CHtiFtpBackupFake implementation
*
*/


// INCLUDE FILES
#include "HtiFtpBackupFake.h"
#include <HtiLogging.h>

// CONSTANTS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiFtpBackupFake* NewHtiFtpBackupFake()
// Function to construct the CHtiFtpBackupFakeBase derived object.
// Exported at ordinal 1 and not a member of the class.
// -----------------------------------------------------------------------------
EXPORT_C CHtiFtpBackupFake* NewHtiFtpBackupFake()
    {
    return new ( ELeave ) CHtiFtpBackupFake();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiFtpBackupFake::ConstructL
// -----------------------------------------------------------------------------
void CHtiFtpBackupFake::ConstructL( RFs* aFs )
    {
    HTI_LOG_FUNC_IN( "CHtiFtpBackupFake::ConstructL" );
    iFs = aFs;
    iSBEClient = NULL;
    HTI_LOG_FUNC_OUT( "CHtiFtpBackupFake::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CHtiFtpBackupFake::~CHtiFtpBackupFake()
// -----------------------------------------------------------------------------
CHtiFtpBackupFake::~CHtiFtpBackupFake()
    {
    HTI_LOG_FUNC_IN( "CHtiFtpBackupFake::~CHtiFtpBackupFake" );
    if ( iSBEClient )
        {
        DeactivateBackup(); // will delete iSBEClient
        }
    delete iSBEClient;  // just to be sure
    iSBEClient = NULL;
    iFs = NULL; // iFs is not owned
    HTI_LOG_FUNC_OUT( "CHtiFtpBackupFake::~CHtiFtpBackupFake" );
    }

// -----------------------------------------------------------------------------
// CHtiFtpBackupFake::ActivateBackup()
// Activates the backup operation if isn't already active.
// -----------------------------------------------------------------------------
TInt CHtiFtpBackupFake::ActivateBackup()
    {
    HTI_LOG_FUNC_IN( "CHtiFtpBackupFake::ActivateBackup" );
    TInt err = KErrNone;
    if ( iSBEClient )
        {
        err = KErrAlreadyExists; // Backup already active
        }
    else
        {
        HTI_LOG_TEXT( "Activating backup" );
        TDriveList drives;
        err = iFs->DriveList( drives );
        if ( !err )
            {
            TRAP( err, iSBEClient = CSBEClient::NewL() );
            if ( !err )
                {
                TRAP( err, iSBEClient->SetBURModeL(
                    drives, EBURBackupFull, EBackupBase ) );
                HTI_LOG_FORMAT( "CSBEClient::SetBURModeL returned %d", err );
                if ( !err )
                    {
                    User::After( 1000000 ); // wait for the backup to activate
                    }
                }
            }
        }
    HTI_LOG_FUNC_OUT( "CHtiFtpBackupFake::ActivateBackup" );
    return err;
    }

// -----------------------------------------------------------------------------
// CHtiFtpBackupFake::DeactivateBackup()
// Deactivates the backup operation if it's active.
// -----------------------------------------------------------------------------
TInt CHtiFtpBackupFake::DeactivateBackup()
    {
    HTI_LOG_FUNC_IN( "CHtiFtpBackupFake::DeactivateBackup" );
    TInt err = KErrNone;

    if ( !iSBEClient )
        {
        err = KErrNotReady; // Backup not active
        }
    else
        {
        HTI_LOG_TEXT( "Deactivating Backup" );
        TDriveList drives;
        err = iFs->DriveList( drives );
        if ( !err )
            {
            TRAP( err, iSBEClient->SetBURModeL(
                drives, EBURNormal, ENoBackup ) );
            HTI_LOG_FORMAT( "CSBEClient::SetBURModeL returned %d", err );
            }
        delete iSBEClient;
        iSBEClient = NULL;
        }

    HTI_LOG_FUNC_OUT( "CHtiFtpBackupFake::DeactivateBackup" );
    return err;
    }

// End of File
